/*
 * hal_lvgl_touch.c
 *
 *  Created on: Dec 15, 2020
 *      Author: patrick
 */
#include "lvgl/lvgl.h"
#include "drv/ft6236u/ft6236u.h"

static lv_indev_drv_t indev_drv;
static lv_indev_t * touch_indev;

static bool hal_lvgl_touch_read(lv_indev_drv_t * drv, lv_indev_data_t*data);

void hal_lvgl_touch_init()
{
	lv_indev_drv_init(&indev_drv);      /*Basic initialization*/

	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = hal_lvgl_touch_read;

	touch_indev = lv_indev_drv_register(&indev_drv);
}

static bool hal_lvgl_touch_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{
	touch_event_t event = touch_get_last_event();
	data->point.x = event.x;
	data->point.y = event.y;
	data->state = ( event.type == TOUCH_EVENT_PRESSED || event .type == TOUCH_EVENT_CONTACT ) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
	return false; /*No buffering now so no more data read*/
}
