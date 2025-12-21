#include "screen_menu.h"
#include "theme.h"
#include "button_edge_bottom.h"
#include <input_devices.h>
#include "ui_images.h"

#include "settings.h"

LV_IMAGE_DECLARE(icon_arrow_back);

typedef enum
{
    LV_MENU_ITEM_BUILDER_VARIANT_1,
    LV_MENU_ITEM_BUILDER_VARIANT_2
} lv_menu_builder_variant_t;

static lv_style_t sty_container;
static lv_style_t sty_menu_header;
static lv_style_t sty_menu_item;
static lv_style_t sty_switch;
static lv_style_t sty_bottom_line;
static lv_style_t sty_button_back;

static lv_obj_t *scr_menu;

/**
 * @brief Handle switch events to update settings
 * @attention Requires user_data to be set to the corresponding settings_id_e value!
 * @param e Event object
 */
static void switch_event_handler(lv_event_t *e)
{
    lv_obj_t *sw = lv_event_get_target(e);
    bool state = lv_obj_has_state(sw, LV_STATE_CHECKED);
    enum settings_id_e setting_id = (enum settings_id_e)lv_event_get_user_data(e);
    settings_set_bool(setting_id, state);
}

static void save_settings_callback(lv_event_t *e)
{
    lv_event_cb_t return_to_home_event_cb = (lv_event_cb_t)lv_event_get_user_data(e);
    save_settings();
    if (return_to_home_event_cb)
    {
        return_to_home_event_cb(e);
    }
}

static void setup_styles(void)
{
    lv_style_init(&sty_container);
    lv_style_set_bg_opa(&sty_container, LV_OPA_TRANSP);
    lv_style_set_border_width(&sty_container, 1);
    lv_style_set_flex_flow(&sty_container, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_cross_place(&sty_container, LV_FLEX_ALIGN_CENTER);
    lv_style_set_layout(&sty_container, LV_LAYOUT_FLEX);
    lv_style_set_height(&sty_container, LV_PCT(100));
    lv_style_set_width(&sty_container, LV_PCT(100));
    lv_style_set_pad_row(&sty_container, 5);
    lv_style_set_pad_ver(&sty_container, 0);

    lv_style_init(&sty_menu_header);
    lv_style_set_text_align(&sty_menu_header, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_font(&sty_menu_header, &font_roboto_bold_24);
    lv_style_set_margin_top(&sty_menu_header, 10);
    lv_style_set_margin_bottom(&sty_menu_header, 5);

    lv_style_init(&sty_menu_item);
    lv_style_set_border_width(&sty_menu_item, 1);
    lv_style_set_width(&sty_menu_item, LV_PCT(100));
    lv_style_set_height(&sty_menu_item, LV_SIZE_CONTENT);
    lv_style_set_layout(&sty_menu_item, LV_LAYOUT_FLEX);
    lv_style_set_flex_flow(&sty_menu_item, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_cross_place(&sty_menu_item, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_main_place(&sty_menu_item, LV_FLEX_ALIGN_CENTER);
    lv_style_set_pad_hor(&sty_menu_item, 10);
    lv_style_set_pad_ver(&sty_menu_item, 8);

    lv_style_init(&sty_switch);
    lv_style_set_height(&sty_switch, 25);
    lv_style_set_width(&sty_switch, 45);
    lv_style_set_align(&sty_switch, LV_ALIGN_BOTTOM_MID);

    lv_style_init(&sty_bottom_line);
    lv_style_set_bg_color(&sty_bottom_line, COLOR_BG_TITLEBAR);
    lv_style_set_border_width(&sty_bottom_line, 0);
    lv_style_set_radius(&sty_bottom_line, 0);
    lv_style_set_height(&sty_bottom_line, 12);
    lv_style_set_width(&sty_bottom_line, LV_PCT(100));
    lv_style_set_align(&sty_bottom_line, LV_ALIGN_BOTTOM_MID);

    lv_style_init(&sty_button_back);
    lv_style_set_bg_color(&sty_button_back, COLOR_BG_TITLEBAR);
    lv_style_set_border_width(&sty_button_back, 0);
    lv_style_set_radius(&sty_button_back, 10);
    lv_style_set_width(&sty_button_back, 120);
    lv_style_set_height(&sty_button_back, 30);
    lv_style_set_text_color(&sty_button_back, COLOR_WHITE);
    lv_style_set_align(&sty_button_back, LV_ALIGN_BOTTOM_MID);
}

static lv_obj_t *create_text(lv_obj_t *parent, const char *icon, const char *txt,
                             lv_menu_builder_variant_t builder_variant)
{
    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_add_style(obj, &sty_menu_item, 0);

    lv_obj_t *img = NULL;
    lv_obj_t *label = NULL;

    if (icon)
    {
        img = lv_image_create(obj);
        lv_image_set_src(img, icon);
    }

    if (txt)
    {
        label = lv_label_create(obj);
        lv_label_set_text(label, txt);
        lv_label_set_long_mode(label, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
        lv_obj_set_flex_grow(label, 1);
    }

    if (builder_variant == LV_MENU_ITEM_BUILDER_VARIANT_2 && icon && txt)
    {
        lv_obj_add_flag(img, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        lv_obj_swap(img, label);
    }

    return obj;
}

static lv_obj_t *create_switch(lv_obj_t *parent, const char *icon, const char *txt, enum settings_id_e setting_id)
{
    lv_obj_t *obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_1);

    lv_obj_t *sw = lv_switch_create(obj);
    bool chk = settings_get_bool(setting_id);
    lv_obj_add_style(sw, &sty_switch, 0);
    lv_obj_add_state(sw, chk ? LV_STATE_CHECKED : LV_STATE_DEFAULT);
    lv_obj_add_event_cb(sw, switch_event_handler, LV_EVENT_VALUE_CHANGED, (void *)setting_id);

    return obj;
}

lv_obj_t *create_menu_screen(lv_event_cb_t return_to_home_event_cb)
{
    setup_styles();

    scr_menu = lv_obj_create(NULL);

    lv_obj_t *menu = lv_obj_create(scr_menu);
    lv_obj_add_style(menu, &sty_container, 0);

    lv_obj_t *header = lv_label_create(menu);
    lv_label_set_text(header, "Station Settings");
    lv_obj_add_style(header, &sty_menu_header, 0);

    create_switch(menu, NULL, "Beep", SETTINGS_ID_BUZZER);

    // Bottom line
    lv_obj_t *line_bottom = lv_obj_create(scr_menu);
    lv_obj_add_style(line_bottom, &sty_bottom_line, 0);

    lv_obj_t *btn_return = button_edge_bottom_create(scr_menu, "Back", &icon_arrow_back_18);
    lv_obj_add_event_cb(btn_return, save_settings_callback, LV_EVENT_CLICKED, return_to_home_event_cb);
    lv_obj_set_ext_click_area(btn_return, 20);

    return scr_menu;
}
