#include "main.h"
#include "delay.h"
#include "adc.h"
#include "tim.h"

/* -------------------------------------------------------------------------- */
/*                                   DEFINES                                  */
/* -------------------------------------------------------------------------- */

#define DEBOUNCE_DELAY_MS 200 // Debounce delay in milliseconds

/* -------------------------------------------------------------------------- */
/*                              TYPE DEFINITIONS                              */
/* -------------------------------------------------------------------------- */

typedef enum
{
    IRON_HW_STATE_IDLE,
    IRON_HW_STATE_ZERO_CROSS,
    IRON_HW_STATE_ENABLE_AMPLIFIER,
    IRON_HW_STATE_PROCESS_ADC,
    IRON_HW_STATE_CONTROL_IRON,
} iron_hardware_state_t;

/* -------------------------------------------------------------------------- */
/*                              STATIC VARIABLES                              */
/* -------------------------------------------------------------------------- */

static iron_hardware_state_t iron_hardware_state = IRON_HW_STATE_IDLE;

static uint32_t debounce_timer = 0;
static _Bool debounce_state = false;
static _Bool debounce_last_state = false;

/* -------------------------------------------------------------------------- */
/*                              STATIC PROTOTYPES                             */
/* -------------------------------------------------------------------------- */

static uint16_t iron_adc_read(ADC_HandleTypeDef *hadc);
static void iron_calculate_temperature(uint16_t adc_value);

/* -------------------------------------------------------------------------- */
/*                              STATIC FUNCTIONS                              */
/* -------------------------------------------------------------------------- */

static void iron_calculate_temperature(uint16_t adc_value)
{
    temperature = (0.1906 * (float)adc_value) + 11.0;

    float alpha = 1.0 / (float)window_size;
    temperature_smooth = (temperature * alpha + temperature_smooth * (1.0 - alpha));
}

static _Bool debounce_sleep_pin()
{
    _Bool current_state = HAL_GPIO_ReadPin(SLEEP_A_GPIO_Port, SLEEP_A_Pin);
    if (current_state != debounce_last_state)
    {
        debounce_timer = HAL_GetTick();
        debounce_last_state = current_state;
    }
    if ((HAL_GetTick() - debounce_timer) > DEBOUNCE_DELAY_MS)
    {
        if (current_state == debounce_last_state)
        {
            debounce_state = current_state;
        }
    }
    return debounce_state;
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
    switch (iron_hardware_state)
    {
    case IRON_HW_STATE_ZERO_CROSS:
        HAL_GPIO_WritePin(HEATER_A_EN_GPIO_Port, HEATER_A_EN_Pin, GPIO_PIN_RESET); // Turn off iron
        half_cycle = half_cycle == 1 ? 0 : 1;                                      // alternate between 0 and 1 to track half cycles
        cycle += half_cycle;                                                       // increase cycle counter on every second half cycle

        iron_sm_state = IRON_HW_STATE_ENABLE_AMPLIFIER; // Next state is to enable thermocouple amplifier
        __HAL_TIM_SET_AUTORELOAD(&htim7, 100);          // 60us
        HAL_TIM_Base_Start_IT(&htim7);
        break;
    case IRON_HW_STATE_ENABLE_AMPLIFIER:
        HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(TCA_AMPLIFIER_EN_GPIO_Port, TCA_AMPLIFIER_EN_Pin, GPIO_PIN_SET);

        iron_sm_state = IRON_HW_STATE_PROCESS_ADC; // Next we process ADC
        __HAL_TIM_SET_AUTORELOAD(&htim7, 300);     // 300us
        HAL_TIM_Base_Start_IT(&htim7);
        break;
    case IRON_HW_STATE_PROCESS_ADC:
        iron_calculate_temperature(iron_adc_read(&hadc1));
        HAL_GPIO_WritePin(TCA_AMPLIFIER_EN_GPIO_Port, TCA_AMPLIFIER_EN_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);

        iron_update_state(); // Update iron status based on iron in stand and sleep timer

        pid.input = (float)temperature_smooth;
        PIDCompute(&pid);
        pid_out = pid.output;
        power = pid_out == 0 ? 0xFFFF : PID_OUT_GRANULARITY + 1 - pid_out;

        iron_sm_state = IRON_HW_STATE_CONTROL_IRON; // Next we process ADC
        __HAL_TIM_SET_AUTORELOAD(&htim7, 50);       // 10us
        HAL_TIM_Base_Start_IT(&htim7);
        break;
    case IRON_HW_STATE_CONTROL_IRON:
        if (iron.state == IRON_STATE_ACTIVE || iron.state == IRON_STATE_SLEEP)
        {
            if (cycle % power == 0)
                HAL_GPIO_WritePin(HEATER_A_EN_GPIO_Port, HEATER_A_EN_Pin, GPIO_PIN_SET);
        }
        iron_sm_state = IRON_HW_STATE_IDLE; // And we are done
        break;
    case IRON_HW_STATE_IDLE:
        Error_Handler(); // This should not happen
        break;
    }
}

void iron_zero_cross_irq_handler()
{
    iron_hardware_state = IRON_HW_STATE_ZERO_CROSS; // Next state is the real zero crossing
    __HAL_TIM_SET_AUTORELOAD(&htim7, 250);    // 300us
    // HAL_TIM_OnePulse_Start_IT(&htim7, 0);
    HAL_TIM_Base_Start_IT(&htim7);
}