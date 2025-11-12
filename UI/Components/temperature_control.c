#include "temperature_control.h"

static lv_style_t sty_container;
static lv_style_t sty_button;
static lv_style_t sty_label;

static void setup_styles()
{
    // Root container styles
    lv_style_init(&sty_container);
    lv_style_set_bg_opa(&sty_container, LV_OPA_TRANSP);
    lv_style_set_border_width(&sty_container, 0);
    lv_style_set_radius(&sty_container, 0);
    lv_style_set_pad_all(&sty_container, 3);
    lv_style_set_height(&sty_container, LV_SIZE_CONTENT);
    lv_style_set_width(&sty_container, LV_SIZE_CONTENT);
    lv_style_set_layout(&sty_container, LV_LAYOUT_FLEX);
    lv_style_set_flex_flow(&sty_container, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_main_place(&sty_container, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(&sty_container, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_track_place(&sty_container, LV_FLEX_ALIGN_CENTER);
    lv_style_set_pad_row(&sty_container, 5);

    // Button styles
    lv_style_init(&sty_button);
    lv_style_set_size(&sty_button, 47, 30);
    lv_style_set_radius(&sty_button, 8);
    lv_style_set_bg_color(&sty_button, lv_color_hex(0xFFFFFF));
    lv_style_set_text_color(&sty_button, lv_color_hex(0x000000));

    // Label styles
    lv_style_init(&sty_label);
    // lv_style_set_border_width(&sty_label, 1); // only for debug
    lv_style_set_text_font(&sty_label, &font_roboto_regular_28_num);
    lv_style_set_margin_hor(&sty_label, 8);
    lv_style_set_pad_bottom(&sty_label, 0);
    lv_style_set_text_align(&sty_label, LV_TEXT_ALIGN_CENTER);
}

lv_obj_t *temperature_control_create(lv_obj_t *parent)
{
    setup_styles();

    lv_obj_t *container = lv_obj_create(parent);
    lv_obj_add_style(container, &sty_container, LV_PART_MAIN);
    // lv_obj_set_user_data(container, user_data);

    /* Button to decrease setpoint */
    lv_obj_t *btn_decrease = lv_button_create(container);
    lv_obj_set_user_data(btn_decrease, (void *)BTN_DECREASE); // Set identifier as user data
    lv_obj_add_style(btn_decrease, &sty_button, LV_PART_MAIN);
    lv_obj_set_style_bg_image_src(btn_decrease, LV_SYMBOL_MINUS, 0);
    lv_obj_add_flag(btn_decrease, LV_OBJ_FLAG_EVENT_BUBBLE);

    /* Temperature setpoint label */
    lv_obj_t *lbl_temperature = lv_label_create(container);
    lv_obj_add_style(lbl_temperature, &sty_label, LV_PART_MAIN);
    lv_obj_align(lbl_temperature, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_align(lbl_temperature, LV_TEXT_ALIGN_CENTER, 0);

    /* Button to increase setpoint */
    lv_obj_t *btn_increase = lv_button_create(container);
    lv_obj_set_user_data(btn_increase, (void *)BTN_INCREASE); // Set identifier as user data
    lv_obj_add_style(btn_increase, &sty_button, 0);
    lv_obj_set_style_bg_image_src(btn_increase, LV_SYMBOL_PLUS, 0);
    lv_obj_add_flag(btn_increase, LV_OBJ_FLAG_EVENT_BUBBLE);

    return container;
}

void temperature_control_set_value(lv_obj_t *obj, int32_t value)
{
    LV_ASSERT_NULL(obj);

    lv_obj_t *lbl_temperature = lv_obj_get_child(obj, 1); // Get the label (second child)
    lv_label_set_text_fmt(lbl_temperature, "%d°C", value);
}

lv_observer_t *temperature_control_bind_setpoint(lv_obj_t *obj, lv_subject_t *subject)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_NULL(subject);

    if (subject->type != LV_SUBJECT_TYPE_INT && subject->type != LV_SUBJECT_TYPE_FLOAT)
    {
        LV_LOG_WARN("Incompatible subject type: %d", subject->type);
        return NULL;
    }

    lv_obj_t *lbl_temperature = lv_obj_get_child(obj, 1); // Get the label (second child)
    lv_observer_t *observer = lv_label_bind_text(lbl_temperature, subject, "%d°C");

    // lv_obj_add_subject_increment_event(obj, subject, LV_EVENT_CLICKED, 1, true);

    return observer;
}