#ifndef IRON_STATUS
#define IRON_STATUS

#include "lvgl.h"

/**
 * Create an iron status component
 *
 * @param parent Pointer to the parent object
 * @return Pointer to the created iron status object
 */
lv_obj_t *iron_status_create(lv_obj_t *parent);

void iron_status_set_status(lv_obj_t *obj, const char *status, const char *msg1, const char *msg2);

/**
 * Bind subjects to the iron status component to automatically update its display
 *
 * @param obj Pointer to the iron status object
 * @param subj_state Pointer to the subject for the iron state
 * @param subj_temperature Pointer to the subject for the temperature value
 * @param subj_hibernate_timer Pointer to the subject for the hibernate timer
 */
void iron_status_bind_status(lv_obj_t *obj, lv_subject_t *subj_state, lv_subject_t *subj_temperature, lv_subject_t *subj_hibernate_timer);

#endif /* IRON_STATUS */
