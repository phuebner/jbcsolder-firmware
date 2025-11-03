#include "iron_viewmodel.h"

iron_viewmodel_t *iron_viewmodel_create(iron_t *iron)
{
    iron_viewmodel_t *viewmodel = malloc(sizeof(iron_viewmodel_t));
    if (!viewmodel)
    {
        return NULL;
    }

    viewmodel->iron = iron;

    // Allocate memory for subjects
    viewmodel->state_subject = malloc(sizeof(lv_subject_t));
    viewmodel->temperature_subject = malloc(sizeof(lv_subject_t));
    viewmodel->setpoint_subject = malloc(sizeof(lv_subject_t));

    lv_subject_init_int(viewmodel->state_subject, iron->state);
    lv_subject_init_float(viewmodel->temperature_subject, iron->temperature);
    lv_subject_init_int(viewmodel->setpoint_subject, iron->setpoint);

    viewmodel->setpoint_inc_event_cb = NULL; // To be assigned later
    viewmodel->setpoint_dec_event_cb = NULL; // To be assigned later

        return viewmodel;
}
