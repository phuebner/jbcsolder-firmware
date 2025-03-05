/*
 * delay.h
 *
 *  Created on: Dec 16, 2020
 *      Author: patrick
 */

#ifndef DELAY
#define DELAY

#include "main.h"

uint32_t Delay_Init();

__STATIC_INLINE void Delay_us(__IO uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    us *= (HAL_RCC_GetHCLKFreq() / 1000000); /* calc number of cycles per us */
    while ((DWT->CYCCNT - start) < us)
        ;
}

__STATIC_INLINE void Delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

#endif /* DELAY */
