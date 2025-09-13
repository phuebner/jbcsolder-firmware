#include "iron_driver_moc.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

// Mock state variables for simulation (channel 1)
static bool ch1_amplifier_enabled = false;
static bool ch1_heater_enabled = false;
static bool ch1_adc_conversion_started = false;
static double ch1_mock_adc_value = 1000.0; // Initial mock ADC value (simulate temperature). Value in double for more realistic simulation. Needs to be converted to uint16_t in adc_read.

void iron_hw_channel_1_amplifier_en(bool en)
{
    ch1_amplifier_enabled = en;
    printf("[MOC] CH1 Amplifier %s\n", en ? "ENABLED" : "DISABLED");
}

void iron_hw_channel_1_heater_en(bool en)
{
    ch1_heater_enabled = en;
    printf("[MOC] CH1 Heater %s\n", en ? "ENABLED" : "DISABLED");
}

void iron_hw_channel_1_adc_start(void)
{
    ch1_adc_conversion_started = true;
    printf("[MOC] CH1 ADC conversion started\n");
}

uint16_t iron_hw_channel_1_adc_read(void)
{
    if (!ch1_adc_conversion_started)
    {
        printf("[MOC] CH1 Warning: ADC read before start\n");
    }
    ch1_adc_conversion_started = false;

    uint16_t adc_value = (uint16_t)round(ch1_mock_adc_value);
    printf("[MOC] CH1 ADC read value: %u\n", adc_value);
    return adc_value;
}

_Bool iron_hw_channel_1_get_sleep_pin_state(void)
{
    // Mock sleep pin state, always return false in this mock
    printf("[MOC] CH1 Sleep pin state: NOT SLEEPING\n");
    return false;
}

iron_drv_t iron_hw_channel_1 = {
    .amplifier_en = iron_hw_channel_1_amplifier_en,
    .heater_en = iron_hw_channel_1_heater_en,
    .adc_start = iron_hw_channel_1_adc_start,
    .adc_read = iron_hw_channel_1_adc_read,
    .get_sleep_pin_state = iron_hw_channel_1_get_sleep_pin_state,
};

void simulate_temperature_change()
{
    if (ch1_heater_enabled)
    {
        // Simulate temperature rising when heater is enabled
        if (ch1_mock_adc_value < 4095.0)
        {
            ch1_mock_adc_value += 0.1;
        }
    }
    else
    {
        // Simulate temperature falling when heater is disabled
        if (ch1_mock_adc_value > 500.0)
        {
            ch1_mock_adc_value -= 0.05;
        }
    }
}