#include "screen_menu.h"
#include "theme.h"
#include "button_edge_bottom.h"
#include <input_devices.h>
#include "ui_images.h"

#include "settings.h"

#define SIDEBAR_RADIUS 30
#define ANIMATION_TIME 300

LV_IMAGE_DECLARE(icon_arrow_back);

typedef enum
{
    LV_MENU_ITEM_BUILDER_VARIANT_1,
    LV_MENU_ITEM_BUILDER_VARIANT_2
} lv_menu_builder_variant_t;

static lv_style_t sty_container;
static lv_style_t sty_sidebar;
static lv_style_t sty_bottom_line;
static lv_style_t sty_button_back;
static lv_style_t sty_menu_header;

static lv_obj_t *scr_menu;
static lv_anim_t a;

static lv_group_t *g2;

static lv_obj_t *root_page;
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
    lv_style_set_border_width(&sty_container, 0);

    lv_style_init(&sty_sidebar);
    lv_style_set_bg_color(&sty_sidebar, COLOR_BG_TITLEBAR);
    lv_style_set_border_width(&sty_sidebar, 0);
    lv_style_set_radius(&sty_sidebar, SIDEBAR_RADIUS);

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

    lv_style_init(&sty_menu_header);
    lv_style_set_text_align(&sty_menu_header, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_font(&sty_menu_header, &font_roboto_regular_24);
}
static lv_obj_t *create_text(lv_obj_t *parent, const char *icon, const char *txt,
                             lv_menu_builder_variant_t builder_variant)
{
    lv_obj_t *obj = lv_menu_cont_create(parent);

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
    lv_obj_add_state(sw, chk ? LV_STATE_CHECKED : LV_STATE_DEFAULT);
    lv_obj_add_event_cb(sw, switch_event_handler, LV_EVENT_VALUE_CHANGED, (void *)setting_id);

    return obj;
}

lv_obj_t *create_menu_screen(lv_event_cb_t return_to_home_event_cb)
{
    setup_styles();

    scr_menu = lv_obj_create(NULL);

    lv_obj_t *line_bottom = lv_obj_create(scr_menu);
    lv_obj_add_style(line_bottom, &sty_bottom_line, 0);

    lv_obj_t *menu = lv_menu_create(scr_menu);
    lv_obj_add_style(menu, &sty_container, 0);
    lv_obj_t *header = lv_menu_get_main_header(menu);
    lv_obj_add_style(header, &sty_menu_header, 0);

    // lv_menu_set_mode_root_back_button(menu, LV_MENU_ROOT_BACK_BUTTON_DISABLED);
    lv_obj_set_size(menu, lv_display_get_horizontal_resolution(NULL), lv_display_get_vertical_resolution(NULL));
    lv_obj_center(menu);

    lv_obj_t *sub_mechanics_page = lv_menu_page_create(menu, "Mechanics");
    // lv_obj_set_style_pad_hor(sub_mechanics_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), LV_PART_MAIN), 0);
    lv_menu_separator_create(sub_mechanics_page);

    lv_obj_t *cont;
    lv_obj_t *section;

    // section = lv_menu_section_create(sub_mechanics_page);

    root_page = lv_menu_page_create(menu, "Station Settings");
    create_switch(root_page, NULL, "Beep", SETTINGS_ID_BUZZER);

    // lv_obj_set_style_pad_hor(root_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), LV_PART_MAIN), 0);
    // section = lv_menu_cont_create(root_page);
    cont = create_text(root_page, NULL, "Mechanics", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(menu, cont, sub_mechanics_page);
    // lv_menu_set_load_page_event(menu, cont, sub_mechanics_page); // const lv_coord_t SIDEBAR_WIDTH = 60;
    // lv_menu_set_sidebar_page(menu, root_page);
    // lv_menu_set_sidebar_page(menu, NULL);
    // lv_menu_clear_history(menu); /* Clear history because we will be showing the root page later */
    lv_menu_set_page(menu, root_page);
    // lv_obj_send_event(lv_obj_get_child(lv_obj_get_child(lv_menu_get_cur_sidebar_page(menu), 0), 0), LV_EVENT_CLICKED,
    //                   NULL);
    // // Main area
    // lv_obj_t *box_menu = lv_obj_create(scr_menu);
    // lv_obj_add_style(box_menu, &sty_container, 0);
    // lv_obj_set_size(box_menu, LV_HOR_RES - SIDEBAR_WIDTH, LV_VER_RES);
    // lv_obj_align(box_menu, LV_ALIGN_RIGHT_MID, 0, 0);

    // // Sidebar

    // lv_obj_t *box_sidebar = lv_obj_create(scr_menu);
    // lv_obj_add_style(box_sidebar, &sty_sidebar, 0);

    // lv_obj_set_size(box_sidebar, SIDEBAR_WIDTH, LV_VER_RES + SIDEBAR_RADIUS); // Increase height to move top radius out of the screen
    // lv_obj_align(box_sidebar, LV_ALIGN_TOP_LEFT, 0, -SIDEBAR_RADIUS);         // Adjust alignment to center the sidebar

    /* Animate sidebar down */

    // lv_anim_init(&a);
    // lv_anim_set_var(&a, box_sidebar);
    // lv_anim_set_time(&a, ANIMATION_TIME);
    // lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    // lv_anim_set_values(&a, -LV_VER_RES, -SIDEBAR_RADIUS);

    // lv_obj_fade_in(box_sidebar, ANIMATION_TIME, 0);

    // Return button
    // lv_obj_t *btn_return = lv_button_create(scr_menu);
    // lv_obj_add_style(btn_return, &sty_button_back, 0);
    lv_obj_t *btn_return = button_edge_bottom_create(scr_menu, "Back", &icon_arrow_back_18);
    lv_obj_set_style_width(btn_return, 140, 0);
    lv_obj_align(btn_return, LV_ALIGN_BOTTOM_MID, 0, 0);

    // lv_theme_apply(btn_return, (lv_theme_style_t)CUSTOM_THEME_SIDEBAR_BTN);

    // const lv_coord_t _corner_radius = lv_obj_get_style_radius(btn_return, LV_OBJ_PART_MAIN); // Get corner radius of button from style

    // lv_obj_set_size(btn_return, SIDEBAR_WIDTH, SIDEBAR_WIDTH);
    // lv_obj_set_pos(btn_return, 0, 0);

    // lv_btn_set_checkable(btn_return, false);
    // lv_btn_set_layout(btn_return, LV_LAYOUT_OFF); // This allows us to align the label of the button manually

    // lv_obj_t *lbl_btn_return = lv_label_create(btn_return);
    // lv_label_set_text(lbl_btn_return, LV_SYMBOL_BARS);
    // lv_obj_align(lbl_btn_return, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(btn_return, save_settings_callback, LV_EVENT_CLICKED, return_to_home_event_cb);
    lv_obj_set_ext_click_area(btn_return, 20);

    // Heading
    // lv_obj_t *label_heading = lv_label_create(scr_menu);
    // lv_label_set_text(label_heading, "Station Settings");
    // // lv_obj_set_style_local_text_font(label_heading, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_roboto_20);
    // lv_obj_align(label_heading, LV_ALIGN_TOP_MID, 0, 10);

    // Menu line
    // lv_obj_t *cont_menu_line = lv_obj_create(scr_menu);
    // lv_obj_set_size(cont_menu_line, LV_HOR_RES - 56, 50);
    // lv_obj_align_to(cont_menu_line, label_heading, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    // lv_cont_set_layout(cont_menu_line, LV_LAYOUT_PRETTY_MID);
    // lv_obj_set_style_local_bg_opa(cont_menu_line, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    // lv_obj_set_style_local_border_width(cont_menu_line, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);

    // lv_obj_t *label_menu_line = lv_label_create(cont_menu_line);
    // lv_label_set_text(label_menu_line, "Beep");

    // lv_obj_t *sw_beep = lv_switch_create(cont_menu_line);
    // lv_obj_add_state(sw_beep, settings_get_bool(SETTINGS_ID_BUZZER) ? LV_STATE_CHECKED : 0);
    // lv_obj_align(sw_beep, LV_ALIGN_CENTER, 0, 0);
    // lv_obj_add_event_cb(sw_beep, switch_event_handler, LV_EVENT_VALUE_CHANGED, (void *)SETTINGS_ID_BUZZER);

    // create_switch(root_page, NULL, "Beep", SETTINGS_ID_BUZZER);

    // lv_obj_set_style_local_bg_color(sw_beep, LV_SWITCH_PART_BG, LV_STATE_DEFAULT, COLOR_BG_PRESET_DRAWER);
    // lv_obj_set_style_local_bg_color(sw_beep, LV_SWITCH_PART_INDIC, LV_STATE_CHECKED, COLOR_BG_TITLEBAR);

    return scr_menu;
}
