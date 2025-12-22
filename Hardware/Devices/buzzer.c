#include "buzzer.h"
#include "settings.h"
#include "stm32f7xx.h"

TIM_HandleTypeDef htim3;

static void Buzzer_Play_Startup_Chime(void);

void Buzzer_Init(void)
{
    // Initialize the GPIO pin for the buzzer
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable the GPIO clock
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_TIM3_CLK_ENABLE();

    // Configure the GPIO pin
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Configure Timer 3
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = (uint32_t)(SystemCoreClock / 1000000) - 1; // 1 MHz timer clock
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 500 - 1; // 4 kHz PWM frequency
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_PWM_Init(&htim3);

    // Configure PWM channel
    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 250; // 50% duty cycle
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2);

    // Start PWM
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0); // 0% duty cycle

    if (settings_get_bool(SETTINGS_ID_BUZZER))
    {
        Buzzer_Play_Startup_Chime();
    }
}

void Buzzer_On(void)
{
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 250); // 50% duty cycle
}

void Buzzer_Off(void)
{
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0); // 0% duty cycle
}

void Buzzer_SetFrequency(uint16_t frequency)
{
    uint32_t period = 1000000 / frequency - 1;
    __HAL_TIM_SET_AUTORELOAD(&htim3, period);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, period / 2); // 50% duty cycle
}

void Buzzer_SetVolume(uint8_t volume)
{
    // Not implemented
}

void Buzzer_Beep(uint16_t duration)
{
    Buzzer_On();
    HAL_Delay(duration);
    Buzzer_Off();
}

static void Buzzer_Play_Startup_Chime(void)
{
    uint16_t melody[] = {523, 659, 783, 1046}; // C5, E5, G5, C6
    uint16_t noteDurations[] = {150, 150, 150, 300};

    for (int i = 0; i < 4; i++)
    {
        Buzzer_SetFrequency(melody[i]);
        Buzzer_On();
        HAL_Delay(noteDurations[i]);
        Buzzer_Off();
        HAL_Delay(50);
    }
    Buzzer_SetFrequency(4000); // Restore default frequency
}