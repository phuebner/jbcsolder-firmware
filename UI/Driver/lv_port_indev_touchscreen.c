/*
 * lv_port_touch.c
 *
 *  Created on: Dec 15, 2020
 *      Author: patrick
 */
#include "lvgl.h"
#include "ft6236u.h"

static lv_indev_t *touch_indev;

static void _touchscreen_read_cb(lv_indev_t *indev, lv_indev_data_t *data);

void lv_port_indev_touchscreen_init()
{
    // Initialize the FT6236 touch controller
    ft6236_touch_io_init();

    touch_indev = lv_indev_create();
    lv_indev_set_type(touch_indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(touch_indev, _touchscreen_read_cb);
}

static void _touchscreen_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    LV_UNUSED(indev);
    touch_event_t event = ft6236_touch_get_last_event();
    data->point.x = event.x;
    data->point.y = event.y;
    data->state = (event.type == TOUCH_EVENT_PRESSED || event.type == TOUCH_EVENT_CONTACT) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}
