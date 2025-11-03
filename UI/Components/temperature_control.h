#ifndef TEMPERATURE_CONTROL
#define TEMPERATURE_CONTROL

#include "lvgl.h"

/**
 * Button identifier set as user data in button objects
 * can be used to differentiate between increase and decrease buttons in event handlers
 */
typedef enum
{
    BTN_INCREASE = 1,
    BTN_DECREASE
} temperature_control_button_t;

/**
 * Temperature control user data structure
 *
 * @param parent Pointer to the parent object
 * @param temperature_subj Pointer to the temperature subject
 * @param event_cb Callback function for button events
 */
lv_obj_t *temperature_control_create(lv_obj_t *parent);

/**
 * Set the temperature control's displayed value
 *
 * @param obj Pointer to the temperature control object
 * @param value The temperature value to display
 */
void temperature_control_set_value(lv_obj_t *obj, int32_t value);

/**
 * Bind an integer or float subject to the temperature control's setpoint label
 *
 * @param obj Pointer to the temperature control object
 * @param subject Pointer to the subject to bind
 * @return Pointer to the created observer
 */
lv_observer_t *temperature_control_bind_setpoint(lv_obj_t *obj, lv_subject_t *subject);

#endif /* TEMPERATURE_CONTROL */
