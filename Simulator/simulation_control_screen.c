#include "simulation_control_screen.h"
#include "lvgl.h"
#include "iron.h"
#include "iron_driver_moc.h"

extern iron_t *iron_a;

static void sleep_mode_event_handler(lv_event_t *e);

void simulation_control_screen_create()
{
    // ======== Simulation Control Screen ========
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_set_size(screen, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(screen, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_all(screen, 10, 0);
    lv_obj_set_style_pad_row(screen, 10, 0);
    lv_obj_set_style_pad_column(screen, 10, 0);

    // Create a label to display simulation status
    lv_obj_t *lbl_status = lv_label_create(screen);
    lv_label_set_text(lbl_status, "Iron A");
    lv_obj_set_style_text_font(lbl_status, &lv_font_montserrat_24, 0);

    // Create buttons to control simulation

    lv_obj_t *lbl_iron_a_sleep = lv_label_create(screen);
    lv_label_set_text(lbl_iron_a_sleep, "Sleep Mode");
    lv_obj_t *sw_iron_a_sleep = lv_switch_create(screen);
    lv_obj_add_event_cb(sw_iron_a_sleep, sleep_mode_event_handler, LV_EVENT_ALL, &iron_a);

    lv_screen_load(screen);
}

static void sleep_mode_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    // iron_t *iron = (iron_t *)lv_event_get_user_data(e);

    if (code == LV_EVENT_VALUE_CHANGED)
    {
        if (obj)
        {
            _Bool state = lv_obj_has_state(obj, LV_STATE_CHECKED);
            iron_a_set_sleep_mode(state);
        }
    }
}