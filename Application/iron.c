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

/* -------------------------------------------------------------------------- */
/*                              STATIC PROTOTYPES                             */
/* -------------------------------------------------------------------------- */

static void iron_update_state();

/* -------------------------------------------------------------------------- */
/*                              STATIC VARIABLES                              */
/* -------------------------------------------------------------------------- */

static iron_t iron;

static float temperature = 0.0;
static float temperature_smooth = 0;

static _Bool sleep_iron_a = false;

static uint16_t window_size = 10;

static uint8_t half_cycle = 0;
static uint16_t cycle = 0;

static uint16_t power = 0xFFFF;
static PIDControl pid;
static float pid_out;

/* -------------------------------------------------------------------------- */
/*                              GLOBAL FUNCTIONS                              */
/* -------------------------------------------------------------------------- */

/**
 * @brief Init instance of soldering iron
 *
 */
void iron_init()
{
	iron.cfg.sleep_temperature = 80;
	iron.cfg.hibernate_delay = 12000; // 2 minutes

	iron.setpoint = 200;
	iron.state = IRON_STATE_NOT_CONNECTED;
	iron.enabled = false;

	PIDInit(&pid, 1.3, 0, 0.01,
			0.01, 0, PID_OUT_GRANULARITY, AUTOMATIC,
			DIRECT);
	PIDSetpointSet(&pid, 0.0);
}

/* ---------------------------- Setter Functions ---------------------------- */

void iron_set_setpoint(uint16_t temperature)
{
	// Constrain temperature
	temperature = (temperature <= IRON_TEMPERATURE_MAX) ? temperature : IRON_TEMPERATURE_MAX;
	temperature = (temperature >= IRON_TEMPERATURE_MIN) ? temperature : IRON_TEMPERATURE_MIN;
	// Set temperature
	iron.setpoint = temperature;
	// Immediately update pid setpoint if iron is active
	if (iron.state == IRON_STATE_ACTIVE)
		PIDSetpointSet(&pid, (float)iron.setpoint);
}

void iron_set_enable(_Bool value)
{
	iron.enabled = value;
}

// void iron_set_state_change_cb(iron_state_change_cb_t cb)
// {
// 	iron.state_change_cb = cb;
// }

/* ---------------------------- Getter Functions ---------------------------- */

uint16_t iron_get_temperature()
{
	return (uint16_t)temperature_smooth;
}

uint16_t iron_get_setpoint()
{
	return (float)iron.setpoint;
}

float iron_get_power()
{
	return pid.output;
}

iron_state_t iron_get_state()
{
	return iron.state;
}

uint32_t iron_get_seconds_till_hibernate()
{
	return (iron.cfg.hibernate_delay - iron.hibernate_timer) / 100;
}

_Bool iron_is_enabled()
{
	return iron.enabled;
}

_Bool iron_is_sleeping()
{
	return sleep_iron_a;
}

/* -------------------------------------------------------------------------- */
/*                              STATIC FUNCTIONS                              */
/* -------------------------------------------------------------------------- */

static void iron_update_state()
{
	// A large ADC value (saturation) indicates that the iron is not connected
	if (temperature > 550.0)
	{
		PIDSetpointSet(&pid, 0.0);
		iron.state = IRON_STATE_NOT_CONNECTED;
		return;
	}

	// If iron is connected but not enabled set state off
	if (iron.enabled == false)
	{
		PIDSetpointSet(&pid, 0.0);
		iron.state = IRON_STATE_OFF;
		iron.hibernate_timer = 0; // Reset the hibernate timer
		return;
	}

	// If connected and enabled decide on state based on sleep pin and update PID accordingly
	if (debounce_sleep_pin())
	{
		// Check if hibernate delay has been reached
		iron_state_t new_state = (iron.hibernate_timer >= iron.cfg.hibernate_delay) ? IRON_STATE_HIBERNATE : IRON_STATE_SLEEP;
		// New setpoint depending on hibernate or sleep state
		float new_setpoint = new_state == IRON_STATE_SLEEP ? (float)iron.cfg.sleep_temperature : 0.0;

		if (new_state != iron.state)
		{
			PIDSetpointSet(&pid, new_setpoint);
			iron.state = new_state;
		}

		// While in sleep state count the hibernate timer
		if (iron.state == IRON_STATE_SLEEP)
			iron.hibernate_timer++;
	}
	else
	{
		iron_state_t new_state = IRON_STATE_ACTIVE;

		if (new_state != iron.state)
		{
			PIDSetpointSet(&pid, (float)iron.setpoint);
			iron.state = new_state;
			iron.hibernate_timer = 0; // Reset the hibernate timer
		}
	}
}
