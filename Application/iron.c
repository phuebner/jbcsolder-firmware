/*
 * iron.c
 *
 *  Created on: Dec 16, 2020
 *      Author: patrick
 */

#include "iron.h"
#include "pid.h"

#include <stdbool.h>

/* -------------------------------------------------------------------------- */
/*                                   DEFINES                                  */
/* -------------------------------------------------------------------------- */

#define PID_OUT_GRANULARITY 20
#define IRON_TEMPERATURE_MAX 450
#define IRON_TEMPERATURE_MIN 50

#define IRON_TEMPERATURE_SMOOTHING_WINDOW 10 // Number of samples to smooth the temperature

/* -------------------------------------------------------------------------- */
/*                              STATIC PROTOTYPES                             */
/* -------------------------------------------------------------------------- */

static void iron_measure_temperature(iron_t *iron);
static void iron_compute_pid(iron_t *iron);

/* -------------------------------------------------------------------------- */
/*                              STATIC VARIABLES                              */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/*                              GLOBAL FUNCTIONS                              */
/* -------------------------------------------------------------------------- */

/**
 * @brief Init instance of soldering iron
 *
 */
void iron_init(iron_t *iron, iron_drv_t *drv)
{
	// Set hardware driver
	iron->drv = drv;

	// Initialize the hardware
	iron->drv->amplifier_en(false);
	iron->drv->heater_en(false);

	// Initialize configuration
	iron->cfg.sleep_temperature = 80;
	iron->cfg.hibernate_delay = 12000; // 2 minutes

	iron->setpoint = 200;
	iron->state = IRON_STATE_NOT_CONNECTED;
	iron->enabled = false;
	iron->hibernate_timer = 0;
	iron->temperature = 0.0;
	iron->temperature_smooth = 0.0;
	iron->power = 0xFFFF; // 0xFFFF means no power, 0 means full power
	iron->pid_out = 0.0;

	PIDInit(&iron->pid, 1.3, 0, 0.01,
			0.01, 0, PID_OUT_GRANULARITY, AUTOMATIC,
			DIRECT);
	PIDSetpointSet(&iron->pid, 0.0);
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
		PIDSetpointSet(&iron->pid, (float)iron->setpoint);
}

void iron_set_enable(iron_t *iron, _Bool value)
{
	iron->enabled = value;
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
	return (float)iron->setpoint;
}

float iron_get_power(iron_t *iron)
{
	return iron->pid.output;
}

iron_state_t iron_get_state(iron_t *iron)
{
	return iron->state;
}

uint32_t iron_get_seconds_till_hibernate(iron_t *iron)
{
	return (iron->cfg.hibernate_delay - iron->hibernate_timer) / 100;
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
	iron->temperature = (0.1906 * (float)adc_value) + 11.0;

	float alpha = 1.0 / (float)IRON_TEMPERATURE_SMOOTHING_WINDOW;
	iron->temperature_smooth = (iron->temperature * alpha + iron->temperature_smooth * (1.0 - alpha));
}

static void iron_compute_pid(iron_t *iron)
{
	// Update PID input with the smoothed temperature
	iron->pid.input = iron->temperature_smooth;

	// Compute PID output
	PIDCompute(&iron->pid);
}

void iron_heater_disable(iron_t *iron)
{
	// Disable the heater
	iron->drv->heater_en(false);
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
	printf("PID Output: %f, Heater Skip Half Cycles: %d\n", iron->pid.output, iron->heater_skip_half_cycles);

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
	if (iron->temperature > 550.0)
	{
		PIDSetpointSet(&iron->pid, 0.0);
		iron->state = IRON_STATE_NOT_CONNECTED;
		return;
	}

	// If iron is connected but not enabled set state off
	if (iron->enabled == false)
	{
		PIDSetpointSet(&iron->pid, 0.0);
		iron->state = IRON_STATE_OFF;
		iron->hibernate_timer = 0; // Reset the hibernate timer
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
			PIDSetpointSet(&iron->pid, new_setpoint);
			iron->state = new_state;
		}

		// While in sleep state count the hibernate timer
		if (iron->state == IRON_STATE_SLEEP)
			iron->hibernate_timer++;
	}
	else
	{
		iron_state_t new_state = IRON_STATE_ACTIVE;

		if (new_state != iron->state)
		{
			PIDSetpointSet(&iron->pid, (float)iron->setpoint);
			iron->state = new_state;
			iron->hibernate_timer = 0; // Reset the hibernate timer
		}
	}

	// Compute the PID output based on the current temperature
	// and the setpoint temperature
	iron_compute_pid(iron);
}
