/*
 * iron.c
 *
 *  Created on: Dec 16, 2020
 *      Author: patrick
 */
#include "main.h"
#include "delay.h"
#include "iron.h"
#include "adc.h"
#include "tim.h"

#include <stdbool.h>

/* -------------------------------------------------------------------------- */
/*                                   DEFINES                                  */
/* -------------------------------------------------------------------------- */

#define PID_OUT_GRANULARITY		20
#define IRON_TEMPERATURE_MAX	450
#define IRON_TEMPERATURE_MIN	50

/* -------------------------------------------------------------------------- */
/*                              STATIC PROTOTYPES                             */
/* -------------------------------------------------------------------------- */

static uint16_t iron_adc_read(ADC_HandleTypeDef *hadc);
static void iron_calculate_temperature(uint16_t adc_value);
static void iron_update_state();

/* -------------------------------------------------------------------------- */
/*                              TYPE DEFINITIONS                              */
/* -------------------------------------------------------------------------- */

typedef enum {
	IRON_SM_STATE_IDLE,
	IRON_SM_STATE_ZERO_CROSS,
	IRON_SM_STATE_ENABLE_AMPLIFIER,
	IRON_SM_STATE_PROCESS_ADC,
	IRON_SM_STATE_CONTROL_IRON,
} iron_statemachine_state_t;

/* -------------------------------------------------------------------------- */
/*                              STATIC VARIABLES                              */
/* -------------------------------------------------------------------------- */

static iron_t iron;
static iron_statemachine_state_t iron_sm_state = IRON_SM_STATE_IDLE;

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
	iron.cfg.hibernate_delay = 6000; // 1 minute

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
	if(iron.state == IRON_STATE_ACTIVE)
		PIDSetpointSet(&pid, (float) iron.setpoint);
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
	return (float) iron.setpoint;
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

static uint16_t iron_adc_read(ADC_HandleTypeDef *hadc)
{
	uint16_t value = 0;
	HAL_ADC_Start(hadc);

	HAL_ADC_PollForConversion(hadc, 1); // 1ms timeout

	if ((HAL_ADC_GetState(hadc) & HAL_ADC_STATE_EOC_REG) == HAL_ADC_STATE_EOC_REG)
	{
		value = HAL_ADC_GetValue(hadc);
	}
	return value;
}

static void iron_calculate_temperature(uint16_t adc_value)
{
	temperature = (0.1906 * (float)adc_value) + 11.0;

	float alpha = 1.0 / (float)window_size;
	temperature_smooth = (temperature * alpha + temperature_smooth * (1.0 - alpha));
}

static void iron_update_state()
{
	// A large ADC value (saturation) indicates that the iron is not connected
	if(temperature > 550.0)
	{
		PIDSetpointSet(&pid, 0.0);
		iron.state = IRON_STATE_NOT_CONNECTED;
		return;
	}

	// If iron is connected but not enabled set state off
	if(iron.enabled == false)
	{	
		PIDSetpointSet(&pid, 0.0);
		iron.state = IRON_STATE_OFF;
		iron.hibernate_timer = 0; // Reset the hibernate timer
		return;
	}

	// If connected and enabled decide on state based on sleep pin and update PID accordingly
	if(HAL_GPIO_ReadPin(SLEEP_A_GPIO_Port, SLEEP_A_Pin) == GPIO_PIN_SET)
	{
		// Check if hibernate delay has been reached
		iron_state_t new_state = (iron.hibernate_timer >= iron.cfg.hibernate_delay) ? IRON_STATE_HIBERNATE : IRON_STATE_SLEEP;
		// New setpoint depending on hibernate or sleep state
		float new_setpoint = new_state == IRON_STATE_SLEEP ? (float) iron.cfg.sleep_temperature : 0.0;

		if(new_state != iron.state)
		{
			PIDSetpointSet(&pid, new_setpoint);
			iron.state = new_state;
		}

		// While in sleep state count the hibernate timer
		if(iron.state == IRON_STATE_SLEEP)
			iron.hibernate_timer++;

	}
	else
	{
		iron_state_t new_state = IRON_STATE_ACTIVE;
		
		if(new_state != iron.state)
		{
			PIDSetpointSet(&pid, (float) iron.setpoint);
			iron.state = new_state;
			iron.hibernate_timer = 0; // Reset the hibernate timer
		}
	}
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
	switch(iron_sm_state)
	{
	case IRON_SM_STATE_ZERO_CROSS:
		HAL_GPIO_WritePin(HEATER_A_EN_GPIO_Port, HEATER_A_EN_Pin, GPIO_PIN_RESET); //Turn off iron
		half_cycle = half_cycle == 1 ? 0 : 1; // alternate between 0 and 1 to track half cycles
		cycle += half_cycle; // increase cycle counter on every second half cycle

		iron_sm_state = IRON_SM_STATE_ENABLE_AMPLIFIER; // Next state is to enable thermocouple amplifier
		__HAL_TIM_SET_AUTORELOAD(&htim7, 100); //60us
		HAL_TIM_Base_Start_IT(&htim7);
		break;
	case IRON_SM_STATE_ENABLE_AMPLIFIER:
		HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(TCA_AMPLIFIER_EN_GPIO_Port, TCA_AMPLIFIER_EN_Pin, GPIO_PIN_SET);

		iron_sm_state = IRON_SM_STATE_PROCESS_ADC; // Next we process ADC
		__HAL_TIM_SET_AUTORELOAD(&htim7, 300); //300us
		HAL_TIM_Base_Start_IT(&htim7);
		break;
	case IRON_SM_STATE_PROCESS_ADC:
		iron_calculate_temperature(iron_adc_read(&hadc1));
		HAL_GPIO_WritePin(TCA_AMPLIFIER_EN_GPIO_Port, TCA_AMPLIFIER_EN_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);

		iron_update_state(); // Update iron status based on iron in stand and sleep timer

		pid.input = (float) temperature_smooth;
		PIDCompute(&pid);
		pid_out = pid.output;
		power = pid_out == 0 ? 0xFFFF : PID_OUT_GRANULARITY + 1 - pid_out;

		iron_sm_state = IRON_SM_STATE_CONTROL_IRON; // Next we process ADC
		__HAL_TIM_SET_AUTORELOAD(&htim7, 50); //10us
		HAL_TIM_Base_Start_IT(&htim7);
		break;
	case IRON_SM_STATE_CONTROL_IRON:
		if (iron.state == IRON_STATE_ACTIVE || iron.state == IRON_STATE_SLEEP) {
			if(cycle % power == 0)
				HAL_GPIO_WritePin(HEATER_A_EN_GPIO_Port, HEATER_A_EN_Pin, GPIO_PIN_SET);
		}
		iron_sm_state = IRON_SM_STATE_IDLE; // And we are done
		break;
	case IRON_SM_STATE_IDLE:
		Error_Handler(); //This should not happen
		break;
	}
}

void iron_zero_cross_irq_handler()
{
	iron_sm_state = IRON_SM_STATE_ZERO_CROSS; //Next state is the real zero crossing
	__HAL_TIM_SET_AUTORELOAD(&htim7, 250); //300us
	HAL_TIM_Base_Start_IT(&htim7);
}
