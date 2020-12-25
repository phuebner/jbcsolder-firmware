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


static uint16_t iron_adc_read(ADC_HandleTypeDef *hadc);
static void iron_calculate_temperature(uint16_t adc_value);

static float temperature = 0.0;
static float temperature_smooth = 0;
static uint16_t temp_setpoint = 200;
static bool heater_en = false;

static _Bool sleep_iron_a = false;

static uint16_t window_size = 20;

typedef enum {
	ST_IDLE,
	ST_ZERO_CROSS,
	ST_ENABLE_AMPLIFIER,
	ST_PROCESS_ADC,
	ST_CONTROL_IRON,
} iron_state_t;

static iron_state_t iron_state = ST_IDLE;

uint16_t iron_temperature()
{
	return (uint8_t)temperature_smooth;
}

_Bool iron_isEnabled()
{
	return heater_en;
}

void iron_setEnable(_Bool value)
{
	heater_en = value;
}



static uint16_t iron_adc_read(ADC_HandleTypeDef *hadc)
{
	uint16_t value = 0;
	HAL_ADC_Start(hadc);

	HAL_ADC_PollForConversion(hadc, 20);

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

/**
 * IRON STATE MACHINE BASED ON TIMER INTERRUPTS
 */
void iron_timer_irq_handler()
{
	switch(iron_state)
	{
	case ST_ZERO_CROSS:
		HAL_GPIO_WritePin(HEATER_A_EN_GPIO_Port, HEATER_A_EN_Pin, GPIO_PIN_RESET); //Turn off iron

//		sleep_iron_a = HAL_GPIO_ReadPin(SLEEP_A_GPIO_Port, SLEEP_A_Pin) == GPIO_PIN_SET;

		iron_state = ST_ENABLE_AMPLIFIER; // Next state is to enable thermocouple amplifier
		__HAL_TIM_SET_AUTORELOAD(&htim7, 20); //20us
		HAL_TIM_Base_Start_IT(&htim7);
		break;
	case ST_ENABLE_AMPLIFIER:
		HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(TCA_AMPLIFIER_EN_GPIO_Port, TCA_AMPLIFIER_EN_Pin, GPIO_PIN_SET);

		iron_state = ST_PROCESS_ADC; // Next we process ADC
		__HAL_TIM_SET_AUTORELOAD(&htim7, 300); //300us
		HAL_TIM_Base_Start_IT(&htim7);
		break;
	case ST_PROCESS_ADC:
		iron_calculate_temperature(iron_adc_read(&hadc1));
		HAL_GPIO_WritePin(TCA_AMPLIFIER_EN_GPIO_Port, TCA_AMPLIFIER_EN_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);

		iron_state = ST_CONTROL_IRON; // Next we process ADC
		__HAL_TIM_SET_AUTORELOAD(&htim7, 10); //10us
		HAL_TIM_Base_Start_IT(&htim7);
		break;
	case ST_CONTROL_IRON:
		if(HAL_GPIO_ReadPin(BUTTON_1_GPIO_Port, BUTTON_1_Pin) == GPIO_PIN_SET)
			heater_en = true;
		else
			heater_en = false;

		if (heater_en == true) {
			if (temperature < temp_setpoint) {
				HAL_GPIO_WritePin(HEATER_A_EN_GPIO_Port, HEATER_A_EN_Pin, GPIO_PIN_SET);
			}
		}
		iron_state = ST_IDLE; // And we are done
		break;
	case ST_IDLE:
		Error_Handler(); //This should not happen
		break;
	}
}

void iron_zero_cross_irq_handler()
{
	iron_state = ST_ZERO_CROSS; //Next state is the real zero crossing
	__HAL_TIM_SET_AUTORELOAD(&htim7, 200); //300us
	HAL_TIM_Base_Start_IT(&htim7);
}
