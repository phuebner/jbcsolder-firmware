/*
 * delay.c
 *
 *  Created on: Dec 16, 2020
 *      Author: patrick
 */

#include "delay.h"



uint32_t Delay_Init()
{
	/* Enable TRC */
    CoreDebug->DEMCR &= ~0x01000000;
    CoreDebug->DEMCR |=  0x01000000;

    /* !!!! on Cortex-M7 the lock register must be written
     * with this magic code in order to unlock the cycle counter */
	DWT->LAR = 0xC5ACCE55;

    /* Enable counter */
    DWT->CTRL &= ~0x00000001;
    DWT->CTRL |=  0x00000001;

    /* Reset counter */
    DWT->CYCCNT = 0;

    /* Check if DWT has started */
	uint32_t start = DWT->CYCCNT;

	/* 2 dummys */
	__asm volatile ("NOP");
	__asm volatile ("NOP");

	/* Return difference, if result is zero, DWT has not started */
	return (DWT->CYCCNT - start);
}
