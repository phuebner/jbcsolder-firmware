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

#include "hal_lvgl_encoder.h"
#include "main.h"
#include "tim.h"

static lv_indev_drv_t indev_drv;

lv_indev_t *enc_indev;

static bool hal_lvgl_encoder_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{
    int32_t enc_diff = ((int16_t)(htim1.Instance->CNT))/2;
    if(enc_diff != 0) {
    	htim1.Instance->CNT = 0; //reset counter
    }

	data->enc_diff = enc_diff;
    data->state = (HAL_GPIO_ReadPin(BUTTON_ENC_GPIO_Port, BUTTON_ENC_Pin) == GPIO_PIN_SET) ?
        LV_INDEV_STATE_PR : 
        LV_INDEV_STATE_REL;

	return false; /*No buffering now so no more data read*/
}

void hal_lvgl_encoder_init()
{
	lv_indev_drv_init(&indev_drv);      /*Basic initialization*/

	indev_drv.type = LV_INDEV_TYPE_ENCODER;
	indev_drv.read_cb = hal_lvgl_encoder_read;

	enc_indev = lv_indev_drv_register(&indev_drv);
}

