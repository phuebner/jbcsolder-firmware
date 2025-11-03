#ifndef POWER_BAR
#define POWER_BAR

#include "lvgl.h"

lv_obj_t *power_bar_create(lv_obj_t *parent);

void power_bar_set_value(lv_obj_t *obj, int32_t value);

/**
 * Bind a integer subject to the power bar's value
 *
 * @param obj Pointer to the power bar object
 * @param subject Pointer to the subject to bind
 * @return Pointer to the created observer
 */
lv_observer_t *power_bar_bind_power(lv_obj_t *obj, lv_subject_t *subject);

#endif /* POWER_BAR */
