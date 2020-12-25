/*
 * iron.h
 *
 *  Created on: Dec 16, 2020
 *      Author: patrick
 */

#ifndef SRC_IRON_H_
#define SRC_IRON_H_

#include "main.h"

uint16_t iron_temperature();
_Bool iron_isEnabled();
void iron_setEnable(_Bool value);

void iron_timer_irq_handler();
void iron_zero_cross_irq_handler();

#endif /* SRC_IRON_H_ */
