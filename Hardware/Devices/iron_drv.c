
#include "iron_drv.h"
#include "adc.h"

/* -------------------------------------------------------------------------- */
/*                              TYPE DEFINITIONS                              */
/* -------------------------------------------------------------------------- */
typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
    uint32_t debounce_delay_ms;
    _Bool last_state;
    uint32_t timer;
    _Bool state;
} debounce_pin_t;

/* -------------------------------------------------------------------------- */
/*                              STATIC VARIABLES                              */
/* -------------------------------------------------------------------------- */
static debounce_pin_t sleep_pin_ch1 = {
    .port = SLEEP_A_GPIO_Port,
    .pin = SLEEP_A_Pin,
    .debounce_delay_ms = DEBOUNCE_DELAY_MS,
    .last_state = 0,
    .timer = 0,
    .state = 0};

/* -------------------------------------------------------------------------- */
/*                              STATIC FUNCTIONS                              */
/* -------------------------------------------------------------------------- */

/* ---------------------------- Shared Functions ---------------------------- */

static uint16_t adc_read(ADC_HandleTypeDef *hadc)
{
    uint16_t value = 0;

    HAL_ADC_PollForConversion(hadc, 3); // 3ms timeout

    if ((HAL_ADC_GetState(hadc) & HAL_ADC_STATE_EOC_REG) == HAL_ADC_STATE_EOC_REG)
    {
        value = HAL_ADC_GetValue(hadc);
    }
    return value;
}

static _Bool debounce_pin(debounce_pin_t *debounce)
{
    _Bool current_state = HAL_GPIO_ReadPin(debounce->port, debounce->pin);
    if (current_state != debounce->last_state)
    {
        debounce->timer = HAL_GetTick();
        debounce->last_state = current_state;
    }
    if ((HAL_GetTick() - debounce->timer) > debounce->debounce_delay_ms)
    {
        if (current_state == debounce->last_state)
        {
            debounce->state = current_state;
        }
    }
    return debounce->state;
}

/* -------------------------------- CHANNEL 1 ------------------------------- */

static void iron_hw_channel_1_amplifier_en(bool enable)
{
    HAL_GPIO_WritePin(TCA_AMPLIFIER_EN_GPIO_Port, TCA_AMPLIFIER_EN_Pin, enable ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void iron_hw_channel_1_heater_en(bool enable)
{
    HAL_GPIO_WritePin(HEATER_A_EN_GPIO_Port, HEATER_A_EN_Pin, enable ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void iron_hw_channel_1_adc_start(void)
{
    HAL_ADC_Start(&hadc1);
}

static uint16_t iron_hw_channel_1_adc_read(void)
{
    return adc_read(&hadc1);
}

static _Bool iron_hw_channel_1_get_sleep_pin_state(void)
{
    return debounce_sleep_pin(&sleep_pin_ch1);
}

iron_drv_t iron_hw_channel_1 = {
    .amplifier_en = iron_hw_channel_1_amplifier_en,
    .heater_en = iron_hw_channel_1_heater_en,
    .adc_start = iron_hw_channel_1_adc_start,
    .adc_read = iron_hw_channel_1_adc_read,
    .get_sleep_pin_state = iron_hw_channel_1_get_sleep_pin_state, // Use the debounce function for sleep pin state
};