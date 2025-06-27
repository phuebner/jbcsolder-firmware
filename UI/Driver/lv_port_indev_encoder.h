/*
 * hal_lvgl_encoder.h
 *
 *  Created on: Dec 30, 2020
 *      Author: patrick
 */

#ifndef LV_PORT_INDEV_ENCODER
#define LV_PORT_INDEV_ENCODER

#include "lvgl.h"

void lv_port_indev_encoder_init();

extern lv_indev_t *enc_indev;
#endif /* LV_PORT_INDEV_ENCODER */
