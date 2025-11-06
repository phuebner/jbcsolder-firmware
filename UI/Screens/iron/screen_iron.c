#include "screen_iron.h"
#include "power_bar.h"
#include "quick_drawer.h"
#include "temperature_control.h"
#include "iron_status.h"
#include "iron_viewmodel.h"
#include <stdio.h>

LV_FONT_DECLARE(lv_font_roboto_80);
LV_FONT_DECLARE(lv_font_roboto_40);

/* --------------------------------- Styles --------------------------------- */
static lv_style_t sty_container;

static void screen_iron_update_timer_cb(lv_timer_t *timer);
// static void temperature_control_event_cb(temp_ctl_evt_type_t evt_type, void *target);
// static void iron_update_observer(iron_t *iron, void *user_data);

static void temperature_control_event_cb(lv_event_t *e);
static void toggle_iron_event_cb(lv_event_t *e);

typedef struct
{
    iron_t *iron;
    lv_subject_t *subj_setpoint;
    lv_subject_t *subj_temperature;
    lv_subject_t *subj_power;
    lv_subject_t *subj_state;
    lv_subject_t *subj_hibernate_timer;
} screen_iron_user_data_t;

static void setup_styles()
{
    lv_style_init(&sty_container);
    lv_style_set_bg_opa(&sty_container, LV_OPA_TRANSP);
    lv_style_set_border_width(&sty_container, 0); // For debugging
    lv_style_set_pad_all(&sty_container, 0);
    lv_style_set_radius(&sty_container, 0);
    lv_style_set_pad_all(&sty_container, 0);
    lv_style_set_height(&sty_container, lv_pct(100));
    lv_style_set_width(&sty_container, lv_pct(100));
}

lv_obj_t *screen_iron_create(lv_obj_t *parent, iron_t *iron)
{
    setup_styles();
    // iron_viewmodel_t *model = iron_viewmodel_create(iron);

    // Initialize user data
    screen_iron_user_data_t *user_data = lv_malloc(sizeof(screen_iron_user_data_t));
    user_data->iron = iron;
    user_data->subj_setpoint = lv_malloc(sizeof(lv_subject_t));
    lv_subject_init_int(user_data->subj_setpoint, iron->setpoint);

    user_data->subj_temperature = lv_malloc(sizeof(lv_subject_t));
    lv_subject_init_float(user_data->subj_temperature, iron->temperature_smooth);

    user_data->subj_power = lv_malloc(sizeof(lv_subject_t));
    lv_subject_init_int(user_data->subj_power, iron->power);

    user_data->subj_state = lv_malloc(sizeof(lv_subject_t));
    lv_subject_init_int(user_data->subj_state, iron_get_state(iron));

    user_data->subj_hibernate_timer = lv_malloc(sizeof(lv_subject_t));
    lv_subject_init_int(user_data->subj_hibernate_timer, 0);

    // iron_observer_add(iron, iron_update_observer, user_data);
    //  iron_viewmodel_t *model = iron_viewmodel_create(iron);

    lv_obj_t *iron_tile = lv_obj_create(parent);
    lv_obj_set_user_data(iron_tile, user_data); // Attach viewmodel to the tile
    lv_obj_add_style(iron_tile, &sty_container, 0);

    lv_obj_set_style_margin_right(iron_tile, QUICK_DRAWER_WIDTH, 0);

    // Set layout to flex
    lv_obj_set_layout(iron_tile, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(iron_tile, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(iron_tile, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END);
    lv_obj_set_style_pad_gap(iron_tile, 0, 0);

    lv_obj_t *power_bar = power_bar_create(iron_tile, 100);
    // Add extra top margin to power bar to account for titlebar button height
    lv_obj_set_style_margin_top(power_bar, lv_obj_get_style_margin_top(power_bar, 0) + 20, 0);
    power_bar_bind_power(power_bar, user_data->subj_power);

    lv_obj_t *container_center = lv_obj_create(iron_tile);
    lv_obj_add_style(container_center, &sty_container, 0);
    // lv_obj_set_width(container_center, LV_SIZE_CONTENT);
    lv_obj_set_style_margin_ver(container_center, 5, LV_PART_MAIN);

    lv_obj_set_flex_grow(container_center, 1);
    // lv_obj_set_layout(container_center, LV_LAYOUT_FLEX);
    // lv_obj_set_flex_flow(container_center, LV_FLEX_FLOW_COLUMN);
    // lv_obj_set_flex_align(container_center, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    /* Label Iron Status */
    lv_obj_t *lbl_iron_name = lv_label_create(container_center);
    // lv_label_set_long_mode(lbl_iron_name, LV_LABEL_LONG_SCROLL_CIRCULAR);
    // lv_obj_set_width(lbl_iron_name, lv_pct(100));
    lv_obj_align(lbl_iron_name, LV_ALIGN_TOP_MID, 0, 0);

    lv_label_set_text(lbl_iron_name, iron->name ? iron->name : "Unnamed");

    lv_obj_t *lbl_iron_type = lv_label_create(container_center);
    lv_label_set_text(lbl_iron_type, iron_type_str[iron->type]);
    lv_obj_align_to(lbl_iron_type, lbl_iron_name, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    /* Current temperature display */
    lv_obj_t *container_current_temp = lv_obj_create(container_center);
    lv_obj_add_style(container_current_temp, &sty_container, LV_PART_MAIN);
    lv_obj_align(container_current_temp, LV_ALIGN_CENTER, 0, -15);
    lv_obj_set_height(container_current_temp, LV_SIZE_CONTENT);
    lv_obj_set_width(container_current_temp, LV_SIZE_CONTENT);
    lv_obj_set_layout(container_current_temp, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(container_current_temp, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(container_current_temp, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_gap(container_current_temp, 0, 0);
    lv_obj_bind_flag_if_not_eq(container_current_temp, user_data->subj_state, LV_OBJ_FLAG_HIDDEN, IRON_STATE_ACTIVE);
    {
        lv_obj_t *lbl_temperature = lv_label_create(container_current_temp);
        lv_obj_set_width(lbl_temperature, 140);
        lv_obj_set_style_text_align(lbl_temperature, LV_TEXT_ALIGN_RIGHT, 0);
        lv_label_bind_text(lbl_temperature, user_data->subj_temperature, "%.0f");

        lv_obj_set_style_text_font(lbl_temperature, &lv_font_roboto_80, 0);

        /* Current Temperature Unit */
        lv_obj_t *lbl_unit = lv_label_create(container_current_temp);
        lv_obj_set_style_text_font(lbl_unit, &lv_font_roboto_40, 0);
        lv_label_set_text(lbl_unit, "°C");
    }

    lv_obj_t *lbl_status = iron_status_create(container_center);
    lv_obj_align(lbl_status, LV_ALIGN_CENTER, 0, 0);
    lv_obj_bind_flag_if_eq(lbl_status, user_data->subj_state, LV_OBJ_FLAG_HIDDEN, IRON_STATE_ACTIVE);
    iron_status_bind_status(lbl_status, user_data->subj_state, user_data->subj_temperature, user_data->subj_hibernate_timer);
    lv_obj_add_event_cb(lbl_status, toggle_iron_event_cb, LV_EVENT_RELEASED, iron);

    lv_obj_t *temp_ctl = temperature_control_create(container_center);
    lv_obj_align(temp_ctl, LV_ALIGN_BOTTOM_MID, 0, 0);
    temperature_control_bind_setpoint(temp_ctl, user_data->subj_setpoint);
    lv_obj_add_event_cb(temp_ctl, temperature_control_event_cb, LV_EVENT_ALL, iron);

    lv_timer_create(screen_iron_update_timer_cb, 100, user_data);

    return iron_tile;
}

void temperature_control_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    // Get the button that was pressed
    lv_obj_t *obj = lv_event_get_target(e);
    temperature_control_button_t btn = (temperature_control_button_t)lv_obj_get_user_data(obj);
    iron_t *iron = (iron_t *)lv_event_get_user_data(e);

    if (iron == NULL)
        return;

    if (btn != BTN_INCREASE && btn != BTN_DECREASE)
        return;

    if (code == LV_EVENT_PRESSED || code == LV_EVENT_LONG_PRESSED_REPEAT)
    {
        switch (btn)
        {
        case BTN_INCREASE:
            iron_set_setpoint(iron, iron_get_setpoint(iron) + 5);
            break;
        case BTN_DECREASE:
            iron_set_setpoint(iron, iron_get_setpoint(iron) - 5);
            break;
        default:
            break;
        }
    }
}

static void toggle_iron_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    iron_t *iron = (iron_t *)lv_event_get_user_data(e);

    if (iron == NULL)
        return;

    if (code == LV_EVENT_RELEASED)
    {
        iron_set_enable(iron, !iron_is_enabled(iron));
    }
}

// static void iron_update_observer(iron_t *iron, void *user_data)
// {
//     if (iron == NULL || user_data == NULL)
//         return;
// }

// Timer callback to update the temperature display
static void screen_iron_update_timer_cb(lv_timer_t *timer)
{
    screen_iron_user_data_t *user_data = (screen_iron_user_data_t *)lv_timer_get_user_data(timer);

    float temperature = iron_get_temperature(user_data->iron);
    lv_subject_set_float(user_data->subj_temperature, temperature);

    uint16_t power = iron_get_power(user_data->iron);
    lv_subject_set_int(user_data->subj_power, power);

    uint16_t setpoint = iron_get_setpoint(user_data->iron);
    if (lv_subject_get_int(user_data->subj_setpoint) != setpoint)
        lv_subject_set_int(user_data->subj_setpoint, setpoint);

    lv_subject_set_int(user_data->subj_state, iron_get_state(user_data->iron));

    lv_subject_set_int(user_data->subj_hibernate_timer, iron_get_seconds_till_hibernate(user_data->iron));
}