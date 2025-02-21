/*
 * iron.c
 *
 *  Created on: Dec 16, 2020
 *      Author: patrick
 */
#include "main.h"
#include "iron.h"
#include <stdbool.h>

/* -------------------------------------------------------------------------- */
/*                              TYPE DEFINITIONS                              */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/*                              STATIC VARIABLES                              */
/* -------------------------------------------------------------------------- */

static iron_t iron;

static float temperature = 0.0;
static float temperature_smooth = 350;

static _Bool sleep_iron_a = false;

static uint16_t window_size = 10;

static uint8_t half_cycle = 0;
static uint16_t cycle = 0;

static uint16_t power = 0xFFFF;

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

	iron.setpoint = 200.0;
	iron.state = IRON_STATE_OFF;
}

/* ---------------------------- Setter Functions ---------------------------- */

void iron_set_setpoint(uint16_t temperature)
{
	iron.setpoint = (float)temperature;
}

void iron_set_enable(_Bool value)
{
	iron.state = value ? IRON_STATE_HIBERNATE : IRON_STATE_OFF;
}

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
	return 10;
}

iron_state_t iron_get_state()
{
	return iron.state;
}

uint32_t iron_get_seconds_till_hibernate()
{
	return 60;
}

_Bool iron_is_enabled()
{
	return iron.state != IRON_STATE_OFF;
}

_Bool iron_is_sleeping()
{
	return sleep_iron_a;
}

/* -------------------------------------------------------------------------- */
/*                             INTERRUPT HANDLERS                             */
/* -------------------------------------------------------------------------- */

/**
 * @brief Iron state machine based on timer interrupt
 *
 */
void iron_timer_irq_handler()
{
}

void iron_zero_cross_irq_handler()
{
}
