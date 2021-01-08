/**
 * @file hal_lvgl_encoder.c
 * @author Patrick Huebner (phuebner86@gmail.com)
 * @brief Driver for lvgl to supply encoder events
 * @version 0.1
 * @date 2020-12-30
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "../Core/Src/hal_lvgl_encoder.h"
#include "main.h"


static lv_indev_drv_t indev_drv;

lv_indev_t *enc_indev;

static bool hal_lvgl_encoder_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{
   	data->enc_diff = 0;
    data->state = LV_INDEV_STATE_REL;

	return false; /*No buffering now so no more data read*/
}

void hal_lvgl_encoder_init()
{
	lv_indev_drv_init(&indev_drv);      /*Basic initialization*/

	indev_drv.type = LV_INDEV_TYPE_ENCODER;
	indev_drv.read_cb = hal_lvgl_encoder_read;

	enc_indev = lv_indev_drv_register(&indev_drv);
}

