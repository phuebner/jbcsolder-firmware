/*
 * hal_lvgl_encoder.h
 *
 *  Created on: Dec 30, 2020
 *      Author: patrick
 */

#ifndef SRC_HAL_LVGL_ENCODER_H_
#define SRC_HAL_LVGL_ENCODER_H_

#include "lvgl/lvgl.h"

void hal_lvgl_encoder_init();

extern lv_indev_t *enc_indev;
#endif /* SRC_HAL_LVGL_ENCODER_H_ */
