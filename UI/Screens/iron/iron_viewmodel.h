#ifndef IRON_VIEWMODEL
#define IRON_VIEWMODEL

#include "lvgl.h"
#include "iron.h"

typedef struct
{
    iron_t *iron;
    lv_subject_t *state_subject;
    lv_subject_t *temperature_subject;
    lv_subject_t *setpoint_subject;

    lv_event_cb_t setpoint_inc_event_cb;
    lv_event_cb_t setpoint_dec_event_cb;
} iron_viewmodel_t;

iron_viewmodel_t *iron_viewmodel_create(iron_t *iron);

#endif /* IRON_VIEWMODEL */
