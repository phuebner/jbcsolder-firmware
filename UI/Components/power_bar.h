#ifndef POWER_BAR
#define POWER_BAR

#include "lvgl.h"

lv_obj_t *power_bar_create(lv_obj_t *parent);

void power_bar_set_value(lv_obj_t *obj, int32_t value);

#endif /* POWER_BAR */
