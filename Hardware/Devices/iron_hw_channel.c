#include "main.h"
#include "iron_hw_channel.h"
#include "adc.h"

/* --------------------------------- Common --------------------------------- */

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

iron_hw_channel_t iron_hw_channel_1 = {
    .amplifier_en = iron_hw_channel_1_amplifier_en,
    .heater_en = iron_hw_channel_1_heater_en,
    .adc_start = iron_hw_channel_1_adc_start,
    .adc_read = iron_hw_channel_1_adc_read,
};