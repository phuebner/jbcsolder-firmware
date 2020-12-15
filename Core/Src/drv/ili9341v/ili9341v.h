/*
 * ili9341v.h
 *
 *  Created on: Dec 12, 2020
 *      Author: patrick
 */

#ifndef SRC_DRV_ILI9341V_ILI9341V_H_
#define SRC_DRV_ILI9341V_ILI9341V_H_

#ifdef __cplusplus
extern "C" {
#endif

#define DISP_FLIPPED 1
#define DISP_HOR  320
#define DISP_VER 240



#include <stdint.h>

void display_init(void);

void display_fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color,
                  void (*operation_complete_callback)());

void display_bitmap(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t *bitmap,
                    void (*operation_complete_callback)());

#ifdef __cplusplus
}
#endif

#endif /* SRC_DRV_ILI9341V_ILI9341V_H_ */
