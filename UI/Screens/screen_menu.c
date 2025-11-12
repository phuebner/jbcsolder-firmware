#include "screen_menu.h"
#include "theme.h"
#include "button_edge_bottom.h"
#include <input_devices.h>
#include "ui_images.h"

#define SIDEBAR_RADIUS 30
#define ANIMATION_TIME 300

LV_IMAGE_DECLARE(icon_arrow_back);

static lv_style_t sty_container;
static lv_style_t sty_sidebar;
static lv_style_t sty_bottom_line;
static lv_style_t sty_button_back;

static lv_obj_t *scr_menu;
static lv_anim_t a;

static lv_group_t *g2;

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
}

lv_obj_t *create_menu_screen(lv_event_cb_t return_to_home_event_cb)
{
    setup_styles();

    scr_menu = lv_obj_create(NULL);

    lv_obj_t *line_bottom = lv_obj_create(scr_menu);
    lv_obj_add_style(line_bottom, &sty_bottom_line, 0);

    // const lv_coord_t SIDEBAR_WIDTH = 60;

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
    lv_obj_add_event_cb(btn_return, return_to_home_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_ext_click_area(btn_return, 20);

    // Heading
    lv_obj_t *label_heading = lv_label_create(scr_menu);
    lv_label_set_text(label_heading, "Station Settings");
    // lv_obj_set_style_local_text_font(label_heading, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_roboto_20);
    lv_obj_align(label_heading, LV_ALIGN_TOP_MID, 0, 10);

    // Menu line
    lv_obj_t *cont_menu_line = lv_obj_create(scr_menu);
    lv_obj_set_size(cont_menu_line, LV_HOR_RES - 56, 50);
    lv_obj_align_to(cont_menu_line, label_heading, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    // lv_cont_set_layout(cont_menu_line, LV_LAYOUT_PRETTY_MID);
    // lv_obj_set_style_local_bg_opa(cont_menu_line, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    // lv_obj_set_style_local_border_width(cont_menu_line, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);

    lv_obj_t *label_menu_line = lv_label_create(cont_menu_line);
    lv_label_set_text(label_menu_line, "Beep");

    lv_obj_t *sw_beep = lv_switch_create(cont_menu_line);
    lv_obj_align(sw_beep, LV_ALIGN_CENTER, 0, 0);
    // lv_obj_set_style_local_bg_color(sw_beep, LV_SWITCH_PART_BG, LV_STATE_DEFAULT, COLOR_BG_PRESET_DRAWER);
    // lv_obj_set_style_local_bg_color(sw_beep, LV_SWITCH_PART_INDIC, LV_STATE_CHECKED, COLOR_BG_TITLEBAR);

    return scr_menu;
}
