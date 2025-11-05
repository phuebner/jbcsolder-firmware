/*
 * iron.c
 *
 *  Created on: Dec 16, 2020
 *      Author: patrick
 */
#include "iron.h"
#include "pid.h"

#include <stdbool.h>
#include <stdio.h>

/* -------------------------------------------------------------------------- */
/*                                   DEFINES                                  */
/* -------------------------------------------------------------------------- */

#define PID_OUT_GRANULARITY 20
#define IRON_TEMPERATURE_MAX 450
#define IRON_TEMPERATURE_MIN 50

#define IRON_TEMPERATURE_SMOOTHING_WINDOW 10 // Number of samples to smooth the temperature

/* -------------------------------------------------------------------------- */
/*                              TYPE DEFINITIONS                              */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/*                              STATIC PROTOTYPES                             */
/* -------------------------------------------------------------------------- */

static void iron_measure_temperature(iron_t *iron);
static void iron_compute_pid(iron_t *iron);
// static void iron_notify_observers(iron_t *iron);

/* -------------------------------------------------------------------------- */
/*                              STATIC VARIABLES                              */
/* -------------------------------------------------------------------------- */
static iron_t iron_instances[IRON_COUNT];
/* -------------------------------------------------------------------------- */
/*                              GLOBAL FUNCTIONS                              */
/* -------------------------------------------------------------------------- */

/**
 * @brief Init instance of soldering iron
 *
 */
iron_t *iron_init(ironIdentifier_e identifier, iron_drv_t *drv, char *name, iron_type_t type)
{
	iron_t *iron = &iron_instances[identifier];

	// Set hardware driver
	iron->drv = drv;

	// Initialize the hardware
	iron->drv->amplifier_en(false);
	iron->drv->heater_en(false);

	// Initialize configuration
	iron->cfg.sleep_temperature = 80;
	iron->cfg.hibernate_delay = 120000; // 2 minutes

	// Initialize other variables
	snprintf(iron->name, sizeof(iron->name), "%s", name);
	iron->type = type;

	iron->setpoint = 200;
	iron->state = IRON_STATE_NOT_CONNECTED;
	iron->enabled = false;
	iron->hibernate_timer = 0;
	iron->temperature = 0.0;
	iron->temperature_smooth = 0.0;
	iron->power = 0xFFFF; // 0xFFFF means no power, 0 means full power
	iron->pid_out = 0.0;

	PIDInit(&(iron->pid), 1.3, 0, 0.01,
			0.01, 0, PID_OUT_GRANULARITY, AUTOMATIC,
			DIRECT);
	PIDSetpointSet(&(iron->pid), 0.0);
	return iron;
}

/* ---------------------------- Setter Functions ---------------------------- */

void iron_set_setpoint(iron_t *iron, uint16_t temperature)
{
	// Constrain temperature
	temperature = (temperature <= IRON_TEMPERATURE_MAX) ? temperature : IRON_TEMPERATURE_MAX;
	temperature = (temperature >= IRON_TEMPERATURE_MIN) ? temperature : IRON_TEMPERATURE_MIN;
	// Set temperature
	iron->setpoint = temperature;
	// Immediately update pid setpoint if iron is active
	if (iron->state == IRON_STATE_ACTIVE)
		PIDSetpointSet(&(iron->pid), (float)iron->setpoint);

	// iron_notify_observers(iron);
}

void iron_set_enable(iron_t *iron, _Bool value)
{
	iron->enabled = value;
	// iron_notify_observers(iron);
}

// void iron_set_state_change_cb(iron_state_change_cb_t cb)
// {
// 	iron.state_change_cb = cb;
// }

/* ---------------------------- Getter Functions ---------------------------- */

uint16_t iron_get_temperature(iron_t *iron)
{
	return (uint16_t)iron->temperature_smooth;
}

uint16_t iron_get_setpoint(iron_t *iron)
{
	return iron->setpoint;
}

float iron_get_power(iron_t *iron)
{
	if (!iron->enabled)
		return 0.0f;
	return iron->pid.output;
}

iron_state_t iron_get_state(iron_t *iron)
{
	return iron->state;
}

uint32_t iron_get_seconds_till_hibernate(iron_t *iron)
{
	return (iron->cfg.hibernate_delay - iron->hibernate_timer) / 1000;
}

_Bool iron_is_enabled(iron_t *iron)
{
	return iron->enabled;
}

_Bool iron_is_sleeping(iron_t *iron)
{
	return (iron->state == IRON_STATE_SLEEP || iron->state == IRON_STATE_HIBERNATE);
}

/* -------------------------------------------------------------------------- */
/*                              STATIC FUNCTIONS                              */
/* -------------------------------------------------------------------------- */

static void iron_measure_temperature(iron_t *iron)
{
	uint16_t adc_value = iron->drv->adc_read();
	iron->temperature = (0.1906f * (float)adc_value) + 11.0f;

	float alpha = 1.0f / (float)IRON_TEMPERATURE_SMOOTHING_WINDOW;
	iron->temperature_smooth = (iron->temperature * alpha + iron->temperature_smooth * (1.0f - alpha));
}

static void iron_compute_pid(iron_t *iron)
{
	// Update PID input with the smoothed temperature
	iron->pid.input = iron->temperature_smooth;

	// Compute PID output
	PIDCompute(&(iron->pid));
}

void iron_control_heater(iron_t *iron)
{
	// Always increase the heater half cycle counter
	iron->half_cycle_counter++;

	// Calculate the number of half cycles to skip for the heater control (i.e. regulate power)
	// If PID output is 0, we skip all half cycles (heater off)
	// If PID output is PID_OUT_GRANULARITY, we skip no half cycles (heater on full power)
	// Otherwise, we calculate the number of half cycles to skip based on the PID output
	iron->heater_skip_half_cycles = iron->pid.output == 0 ? 0xFFFF : (uint16_t)(PID_OUT_GRANULARITY + 1 - iron->pid.output);
	// printf("PID Output: %f, Heater Skip Half Cycles: %d\n", iron->pid.output, iron->heater_skip_half_cycles);

	// Enable or disable the amplifier based on the state
	if (iron->state == IRON_STATE_ACTIVE || iron->state == IRON_STATE_SLEEP)
	{
		if (iron->half_cycle_counter % iron->heater_skip_half_cycles == 0)
		{
			// Enable the heater
			iron->drv->heater_en(true);
		}
	}
}

void iron_update_state(iron_t *iron)
{
	// Read the current temperature from the ADC
	// This will also update the temperature_smooth value
	iron_measure_temperature(iron);

	// A large ADC value (saturation) indicates that the iron is not connected
	if (iron->temperature > 550.0f)
	{
		PIDSetpointSet(&(iron->pid), 0.0f);
		if (iron->state != IRON_STATE_NOT_CONNECTED)
		{
			iron->state = IRON_STATE_NOT_CONNECTED;
			// iron_notify_observers(iron);
		}
		return;
	}

	// If iron is connected but not enabled set state off
	if (iron->enabled == false)
	{
		if (iron->state != IRON_STATE_OFF)
		{
			PIDSetpointSet(&(iron->pid), 0.0f);
			iron->state = IRON_STATE_OFF;
			iron->hibernate_timer = 0; // Reset the hibernate timer
									   // iron_notify_observers(iron);
		}
		return;
	}

	// If connected and enabled decide on state based on sleep pin and update PID accordingly
	if (iron->drv->get_sleep_pin_state())
	{
		// Check if hibernate delay has been reached
		iron_state_t new_state = (iron->hibernate_timer >= iron->cfg.hibernate_delay) ? IRON_STATE_HIBERNATE : IRON_STATE_SLEEP;
		// New setpoint depending on hibernate or sleep state
		float new_setpoint = new_state == IRON_STATE_SLEEP ? (float)iron->cfg.sleep_temperature : (float)0.0;

		if (new_state != iron->state)
		{
			PIDSetpointSet(&(iron->pid), new_setpoint);
			iron->state = new_state;
			// iron_notify_observers(iron);
		}

		// While in sleep state count the hibernate timer
		if (iron->state == IRON_STATE_SLEEP)
		{
			iron->hibernate_timer += 10; // Increase by 10 ms (assuming this function is called every 10 ms)
		}
	}
	else
	{
		iron_state_t new_state = IRON_STATE_ACTIVE;

		if (new_state != iron->state)
		{
			PIDSetpointSet(&(iron->pid), (float)iron->setpoint);
			iron->state = new_state;
			iron->hibernate_timer = 0; // Reset the hibernate timer
									   // iron_notify_observers(iron);
		}
	}

	// Compute the PID output based on the current temperature
	// and the setpoint temperature
	iron_compute_pid(iron);
}

// uint8_t iron_observer_add(iron_t *iron, iron_observer_cb_t cb, void *user_data)
// {
// 	if (iron->observer_count >= IRON_MAX_OBSERVERS)
// 		return 0;

// 	iron->observers[iron->observer_count].cb = cb;
// 	iron->observers[iron->observer_count].user_data = user_data;
// 	iron->observer_count++;
// 	return iron->observer_count;
// }

// uint8_t iron_observer_remove(iron_t *iron, iron_observer_cb_t cb)
// {
// 	for (uint8_t i = 0; i < iron->observer_count; i++)
// 	{
// 		if (iron->observers[i].cb == cb)
// 		{
// 			// Shift remaining observers down
// 			for (uint8_t j = i; j < iron->observer_count - 1; j++)
// 			{
// 				iron->observers[j].cb = iron->observers[j + 1].cb;
// 				iron->observers[j].user_data = iron->observers[j + 1].user_data;
// 			}
// 			iron->observers[--iron->observer_count].cb = NULL;
// 			iron->observers[iron->observer_count].user_data = NULL;
// 			return 1; // Successfully removed
// 		}
// 	}
// 	return 0; // Observer not found
// }

// /**
//  * Notify all registered observers about a state change
//  *
//  * @param iron Pointer to the soldering iron instance
//  */
// void iron_notify_observers(iron_t *iron)
// {
// 	for (uint8_t i = 0; i < iron->observer_count; i++)
// 	{
// 		if (iron->observers[i].cb != NULL)
// 		{
// 			iron->observers[i].cb(iron, iron->observers[i].user_data);
// 		}
// 	}
// }