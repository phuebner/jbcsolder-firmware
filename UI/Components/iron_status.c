#include "iron_status.h"
#include "iron.h"

LV_FONT_DECLARE(lv_font_roboto_18);
LV_FONT_DECLARE(lv_font_roboto_40);

static lv_style_t sty_container;
static lv_style_t sty_lbl_status;
static lv_style_t sty_lbl_msg;

typedef enum
{
    SUBJ_STATE = 0,
    SUBJ_TEMPERATURE,
    SUBJ_HIBERNATE_TIME,
} subj_group_indices_t;

typedef struct
{
    lv_subject_t *subj_group_state_array[3];
    lv_subject_t *subj_group_state;
} iron_state_user_data_t;

static void iron_status_observer_cb(lv_observer_t *observer, lv_subject_t *subject);

static void setup_styles()
{
    lv_style_init(&sty_container);
    lv_style_set_bg_opa(&sty_container, LV_OPA_TRANSP);
    lv_style_set_border_width(&sty_container, 0);
    lv_style_set_radius(&sty_container, 0);
    lv_style_set_pad_all(&sty_container, 0);
    lv_style_set_height(&sty_container, LV_SIZE_CONTENT);
    lv_style_set_width(&sty_container, LV_SIZE_CONTENT);
    lv_style_set_layout(&sty_container, LV_LAYOUT_FLEX);
    lv_style_set_flex_flow(&sty_container, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_main_place(&sty_container, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(&sty_container, LV_FLEX_ALIGN_CENTER);

    lv_style_init(&sty_lbl_status);
    lv_style_set_text_font(&sty_lbl_status, &lv_font_roboto_40);

    lv_style_init(&sty_lbl_msg);
    lv_style_set_text_font(&sty_lbl_msg, &lv_font_roboto_18);
}

lv_obj_t *iron_status_create(lv_obj_t *parent)
{
    setup_styles();

    lv_obj_t *container = lv_obj_create(parent);
    lv_obj_add_style(container, &sty_container, LV_PART_MAIN);

    iron_state_user_data_t *user_data = lv_malloc_zeroed(sizeof(iron_state_user_data_t));
    lv_obj_set_user_data(container, user_data);

    lv_obj_t *lbl_status = lv_label_create(container);
    lv_obj_add_style(lbl_status, &sty_lbl_status, LV_PART_MAIN);
    lv_label_set_text(lbl_status, "");

    lv_obj_t *lbl_msg1 = lv_label_create(container);
    lv_obj_add_style(lbl_msg1, &sty_lbl_msg, LV_PART_MAIN);
    lv_label_set_text(lbl_msg1, "");

    lv_obj_t *lbl_msg2 = lv_label_create(container);
    lv_obj_add_style(lbl_msg2, &sty_lbl_msg, LV_PART_MAIN);
    lv_label_set_text(lbl_msg2, "");

    return container;
}

void iron_status_set_status(lv_obj_t *obj, const char *status, const char *msg1, const char *msg2)
{
    lv_obj_t *lbl_status = lv_obj_get_child(obj, 0);
    lv_obj_t *lbl_msg1 = lv_obj_get_child(obj, 1);
    lv_obj_t *lbl_msg2 = lv_obj_get_child(obj, 2);

    lv_label_set_text(lbl_status, status ? status : "");
    lv_label_set_text(lbl_msg1, msg1 ? msg1 : "");
    lv_label_set_text(lbl_msg2, msg2 ? msg2 : "");
}

void iron_status_bind_status(lv_obj_t *obj, lv_subject_t *subj_state, lv_subject_t *subj_temperature, lv_subject_t *subj_hibernate_time)
{
    iron_state_user_data_t *user_data = lv_obj_get_user_data(obj);
    user_data->subj_group_state_array[SUBJ_STATE] = subj_state;
    user_data->subj_group_state_array[SUBJ_TEMPERATURE] = subj_temperature;
    user_data->subj_group_state_array[SUBJ_HIBERNATE_TIME] = subj_hibernate_time;
    user_data->subj_group_state = lv_malloc(sizeof(lv_subject_t));

    lv_subject_init_group(user_data->subj_group_state, user_data->subj_group_state_array, 3);
    lv_subject_add_observer_obj(user_data->subj_group_state, iron_status_observer_cb, obj, NULL);
}

void iron_status_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
    iron_state_t state = lv_subject_get_int(lv_subject_get_group_element(subject, SUBJ_STATE));
    int32_t temperature = lv_subject_get_float(lv_subject_get_group_element(subject, SUBJ_TEMPERATURE));
    int32_t hibernate_time = lv_subject_get_int(lv_subject_get_group_element(subject, SUBJ_HIBERNATE_TIME));

    lv_obj_t *obj = lv_observer_get_target(observer); // The iron status object
    lv_obj_t *lbl_status = lv_obj_get_child(obj, 0);
    lv_obj_t *lbl_msg1 = lv_obj_get_child(obj, 1);
    lv_obj_t *lbl_msg2 = lv_obj_get_child(obj, 2);

    switch (state)
    {
    case IRON_STATE_NOT_CONNECTED:
        lv_label_set_text(lbl_status, state_str[IRON_STATE_NOT_CONNECTED]);
        lv_label_set_text(lbl_msg1, "");
        lv_label_set_text(lbl_msg2, "");
        break;
    case IRON_STATE_OFF:
        lv_label_set_text(lbl_status, state_str[IRON_STATE_OFF]);
        lv_label_set_text_fmt(lbl_msg1, "Temperature: %d°C", temperature);
        lv_label_set_text(lbl_msg2, "");
        break;
    case IRON_STATE_SLEEP:
        lv_label_set_text(lbl_status, state_str[IRON_STATE_SLEEP]);
        lv_label_set_text_fmt(lbl_msg1, "Temperature: %d°C", temperature);
        lv_label_set_text_fmt(lbl_msg2, "Hibernate in %d:%02d", hibernate_time / 60, hibernate_time % 60);
        break;
    case IRON_STATE_HIBERNATE:
        lv_label_set_text(lbl_status, state_str[IRON_STATE_HIBERNATE]);
        lv_label_set_text_fmt(lbl_msg1, "Temperature: %d°C", temperature);
        lv_label_set_text(lbl_msg2, "");
        break;
    case IRON_STATE_ACTIVE:
        lv_label_set_text(lbl_status, state_str[IRON_STATE_ACTIVE]);
        lv_label_set_text_fmt(lbl_msg1, "Temperature: %d°C", temperature);
        lv_label_set_text(lbl_msg2, "");
        break;
    default:
    }
}

// /* Box Sleep */
// box_sleep = lv_obj_create(box_center, NULL);
// lv_obj_clean_style_list(box_sleep, LV_OBJ_PART_MAIN);
// lv_obj_set_size(box_sleep, CENTER_AREA_WIDTH, 120);
// lv_obj_align(box_sleep, NULL, LV_ALIGN_IN_TOP_MID, 0, 37);
// lv_obj_set_parent_event(box_sleep, true);
// lv_obj_set_drag_parent(box_sleep, true);

// /* Label Iron State Title */
// lbl_iron_state_title = lv_label_create(box_sleep, NULL);
// lv_label_set_long_mode(lbl_iron_state_title, LV_LABEL_LONG_CROP);
// lv_label_set_align(lbl_iron_state_title, LV_LABEL_ALIGN_CENTER);
// lv_obj_set_size(lbl_iron_state_title, CENTER_AREA_WIDTH, 50);
// lv_label_set_text(lbl_iron_state_title, "");
// lv_obj_align(lbl_iron_state_title, NULL, LV_ALIGN_IN_LEFT_MID, 0, -25);
// lv_obj_set_style_local_text_font(lbl_iron_state_title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_roboto_40);
// lv_obj_set_click(lbl_iron_state_title, true);
// lv_obj_set_event_cb(lbl_iron_state_title, main_screen_iron_enable_event_cb);
// lv_obj_set_drag_parent(lbl_iron_state_title, true);
// /* Label Iron State_MSG1 */
// lbl_iron_state_msg1 = lv_label_create(box_sleep, NULL);
// lv_label_set_long_mode(lbl_iron_state_msg1, LV_LABEL_LONG_CROP);
// lv_label_set_align(lbl_iron_state_msg1, LV_LABEL_ALIGN_CENTER);
// lv_obj_set_size(lbl_iron_state_msg1, CENTER_AREA_WIDTH, 20);
// lv_label_set_text(lbl_iron_state_msg1, "");
// lv_obj_align(lbl_iron_state_msg1, NULL, LV_ALIGN_IN_LEFT_MID, 0, 10);
// lv_obj_set_style_local_text_font(lbl_iron_state_msg1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_roboto_18);
// lv_obj_set_drag_parent(lbl_iron_state_msg1, true);
// /* Label Iron State_MSG2 */
// lbl_iron_state_msg2 = lv_label_create(box_sleep, NULL);
// lv_label_set_long_mode(lbl_iron_state_msg2, LV_LABEL_LONG_CROP);
// lv_label_set_align(lbl_iron_state_msg2, LV_LABEL_ALIGN_CENTER);
// lv_obj_set_size(lbl_iron_state_msg2, CENTER_AREA_WIDTH, 20);
// lv_label_set_text(lbl_iron_state_msg2, "");
// lv_obj_align(lbl_iron_state_msg2, NULL, LV_ALIGN_IN_LEFT_MID, 0, 35);
// lv_obj_set_style_local_text_font(lbl_iron_state_msg2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_roboto_18);
// lv_obj_set_drag_parent(lbl_iron_state_msg2, true);