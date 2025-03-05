#include "screen_menu.h"
#include "theme.h"

lv_obj_t *create_menu_screen(lv_event_cb_t return_to_home_event_cb)
{
    lv_obj_t *scr_menu = lv_obj_create(NULL, NULL);
    lv_scr_load(scr_menu);

    const lv_coord_t SIDEBAR_WIDTH = 60;

    // Main area
    lv_obj_t *box_menu = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(box_menu, LV_HOR_RES - SIDEBAR_WIDTH, LV_VER_RES);
    lv_obj_align(box_menu, NULL, LV_ALIGN_IN_RIGHT_MID, 0, 0);
    lv_obj_set_style_local_bg_opa(box_menu, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_set_style_local_border_width(box_menu, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);

    // Sidebar
    const lv_coord_t SIDEBAR_RADIUS = 30;

    lv_obj_t *box_sidebar = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(box_sidebar, SIDEBAR_WIDTH, LV_VER_RES + SIDEBAR_RADIUS);  // Increase height to move top radius out of the screen
    lv_obj_align(box_sidebar, NULL, LV_ALIGN_IN_TOP_LEFT, 0, -SIDEBAR_RADIUS); // Adjust alignment to center the sidebar
    lv_obj_set_style_local_bg_color(box_sidebar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, COLOR_BG_TITLEBAR);
    lv_obj_set_style_local_radius(box_sidebar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, SIDEBAR_RADIUS);

    /* Animate sidebar down */
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, box_sidebar);
    lv_anim_set_time(&a, ANIMATION_TIME);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&a, -LV_VER_RES, -SIDEBAR_RADIUS);
    lv_anim_start(&a);

    lv_obj_fade_in(box_sidebar, ANIMATION_TIME, 0);

    // Return button
    lv_obj_t *btn_return = lv_btn_create(lv_scr_act(), NULL);
    lv_theme_apply(btn_return, (lv_theme_style_t)CUSTOM_THEME_SIDEBAR_BTN);

    // const lv_coord_t _corner_radius = lv_obj_get_style_radius(btn_return, LV_OBJ_PART_MAIN); // Get corner radius of button from style

    lv_obj_set_size(btn_return, SIDEBAR_WIDTH, SIDEBAR_WIDTH);
    lv_obj_set_pos(btn_return, 0, 0);

    lv_btn_set_checkable(btn_return, false);
    lv_btn_set_layout(btn_return, LV_LAYOUT_OFF); // This allows us to align the label of the button manually

    lv_obj_t *lbl_btn_return = lv_label_create(btn_return, NULL);
    lv_label_set_text(lbl_btn_return, LV_SYMBOL_BARS);
    lv_obj_align(lbl_btn_return, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(btn_return, return_to_home_event_cb);
    lv_obj_set_ext_click_area(btn_return, 0, 20, 0, 20);

    // Heading
    lv_obj_t *label_heading = lv_label_create(box_menu, NULL);
    lv_label_set_text(label_heading, "Station Settings");
    lv_obj_set_style_local_text_font(label_heading, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_roboto_20);
    lv_obj_align(label_heading, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);

    // Menu line
    lv_obj_t *cont_menu_line = lv_cont_create(box_menu, NULL);
    lv_obj_set_size(cont_menu_line, LV_HOR_RES - 56, 50);
    lv_obj_align(cont_menu_line, label_heading, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_cont_set_layout(cont_menu_line, LV_LAYOUT_PRETTY_MID);
    lv_obj_set_style_local_bg_opa(cont_menu_line, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_set_style_local_border_width(cont_menu_line, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);

    lv_obj_t *label_menu_line = lv_label_create(cont_menu_line, NULL);
    lv_label_set_text(label_menu_line, "Beep");

    lv_obj_t *sw_beep = lv_switch_create(cont_menu_line, NULL);
    lv_obj_align(sw_beep, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_local_bg_color(sw_beep, LV_SWITCH_PART_BG, LV_STATE_DEFAULT, COLOR_BG_PRESET_DRAWER);
    lv_obj_set_style_local_bg_color(sw_beep, LV_SWITCH_PART_INDIC, LV_STATE_CHECKED, COLOR_BG_TITLEBAR);

    return scr_menu;
}
