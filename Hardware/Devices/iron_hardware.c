#include "main.h"
#include "delay.h"
#include "adc.h"
#include "tim.h"
#include "iron.h"
#include <stdbool.h>

/* -------------------------------------------------------------------------- */
/*                                   DEFINES                                  */
/* -------------------------------------------------------------------------- */

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

static uint32_t half_cycle = 0;
static uint32_t cycle = 0;

/* -------------------------------------------------------------------------- */
/*                              STATIC PROTOTYPES                             */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/*                              STATIC FUNCTIONS                              */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/*                             INTERRUPT HANDLERS                             */
/* -------------------------------------------------------------------------- */
extern iron_t *iron_a;

/**
 * @brief Iron state machine based on timer interrupt
 *
 */
void iron_timer_irq_handler()
{
    if (iron_a == NULL)
        return;

    switch (iron_hardware_state)
    {
    case IRON_HW_STATE_ZERO_CROSS:
        iron_heater_disable(iron_a); // Disable the heater before starting a new cycle

        // Track half-cycles and cycles to control the heater in alternating periods
        half_cycle = half_cycle == 1 ? 0 : 1; // alternate between 0 and 1 to track half cycles
        cycle += half_cycle;                  // increase cycle counter on every second half cycle

        iron_hardware_state = IRON_HW_STATE_ENABLE_AMPLIFIER; // Next state is to enable thermocouple amplifier
        __HAL_TIM_SET_AUTORELOAD(&htim7, 100);                // 100us
        HAL_TIM_Base_Start_IT(&htim7);
        break;
    case IRON_HW_STATE_ENABLE_AMPLIFIER:
        // HAL_GPIO_WritePin(TCA_AMPLIFIER_EN_GPIO_Port, TCA_AMPLIFIER_EN_Pin, GPIO_PIN_SET);
        iron_a->drv->amplifier_en(true); // Enable thermocouple amplifier to read temperature
        iron_a->drv->adc_start();        // Start ADC conversion to read the temperature

        iron_hardware_state = IRON_HW_STATE_PROCESS_ADC; // Next we process ADC
        __HAL_TIM_SET_AUTORELOAD(&htim7, 300);           // 300us
        HAL_TIM_Base_Start_IT(&htim7);
        break;
    case IRON_HW_STATE_PROCESS_ADC:

        iron_a->drv->amplifier_en(false); // Disable thermocouple amplifier to reduce noise
        iron_update_state(iron_a);        // Update iron status based on iron in stand and sleep timer

        iron_hardware_state = IRON_HW_STATE_CONTROL_IRON; // Next we process ADC
        __HAL_TIM_SET_AUTORELOAD(&htim7, 50);             // 10us
        HAL_TIM_Base_Start_IT(&htim7);
        break;
    case IRON_HW_STATE_CONTROL_IRON:
        iron_control_heater(iron_a); // Control the heater based on the current state and temperature

        iron_hardware_state = IRON_HW_STATE_IDLE; // And we are done
        break;
    case IRON_HW_STATE_IDLE:
        Error_Handler(); // This should not happen
        break;
    }
}

void iron_zero_cross_irq_handler()
{
    iron_hardware_state = IRON_HW_STATE_ZERO_CROSS; // Next state is the real zero crossing
    __HAL_TIM_SET_AUTORELOAD(&htim7, 250);          // 300us
    // HAL_TIM_OnePulse_Start_IT(&htim7, 0);
    HAL_TIM_Base_Start_IT(&htim7);
}