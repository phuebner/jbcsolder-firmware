/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BUTTON_ENC_Pin GPIO_PIN_0
#define BUTTON_ENC_GPIO_Port GPIOA
#define ADC_TCA_IN_Pin GPIO_PIN_1
#define ADC_TCA_IN_GPIO_Port GPIOA
#define LCD_BACKLIGHT_Pin GPIO_PIN_5
#define LCD_BACKLIGHT_GPIO_Port GPIOA
#define TCA_AMPLIFIER_EN_Pin GPIO_PIN_7
#define TCA_AMPLIFIER_EN_GPIO_Port GPIOA
#define TS_INT_Pin GPIO_PIN_2
#define TS_INT_GPIO_Port GPIOB
#define TS_INT_EXTI_IRQn EXTI2_IRQn
#define BUTTON_1_Pin GPIO_PIN_11
#define BUTTON_1_GPIO_Port GPIOD
#define BUTTON_2_Pin GPIO_PIN_12
#define BUTTON_2_GPIO_Port GPIOD
#define BUTTON_3_Pin GPIO_PIN_13
#define BUTTON_3_GPIO_Port GPIOD
#define SLEEP_A_Pin GPIO_PIN_8
#define SLEEP_A_GPIO_Port GPIOC
#define SLEEP_B_Pin GPIO_PIN_9
#define SLEEP_B_GPIO_Port GPIOC
#define ENC_A_Pin GPIO_PIN_8
#define ENC_A_GPIO_Port GPIOA
#define ENC_B_Pin GPIO_PIN_9
#define ENC_B_GPIO_Port GPIOA
#define LED_RED_Pin GPIO_PIN_10
#define LED_RED_GPIO_Port GPIOA
#define HEATER_A_EN_Pin GPIO_PIN_11
#define HEATER_A_EN_GPIO_Port GPIOC
#define LED_GREEN_Pin GPIO_PIN_2
#define LED_GREEN_GPIO_Port GPIOD
#define TS_RST_Pin GPIO_PIN_0
#define TS_RST_GPIO_Port GPIOE
#define ZERO_CROSS_Pin GPIO_PIN_1
#define ZERO_CROSS_GPIO_Port GPIOE
#define ZERO_CROSS_EXTI_IRQn EXTI1_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
