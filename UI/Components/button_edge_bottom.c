/**
 * @file button_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "button_edge_bottom.h"
#include "theme.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t *button_edge_bottom_create(lv_obj_t *parent, const char *label, const void *icon)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;
    static lv_style_t style_div;
    static lv_style_t style_pressed;
    static lv_style_t style_box;
    static lv_style_t style_icon;

    static bool style_inited = false;

    if (!style_inited)
    {
        lv_style_init(&style_base);
        lv_style_set_radius(&style_base, 15);
        lv_style_set_bg_opa(&style_base, (255 * 100 / 100));
        lv_style_set_width(&style_base, 120);
        lv_style_set_height(&style_base, 30);
        lv_style_set_pad_hor(&style_base, 0);
        lv_style_set_pad_ver(&style_base, 0);
        lv_style_set_text_font(&style_base, &font_roboto_regular_18);
        lv_style_set_bg_color(&style_base, COLOR_BG_TITLEBAR);
        lv_style_set_text_color(&style_base, COLOR_WHITE);

        lv_style_init(&style_div);
        lv_style_set_radius(&style_div, 0);
        lv_style_set_bg_opa(&style_div, (255 * 0 / 100));
        lv_style_set_width(&style_div, LV_SIZE_CONTENT);
        lv_style_set_height(&style_div, LV_SIZE_CONTENT);
        lv_style_set_pad_hor(&style_div, 4);
        lv_style_set_pad_ver(&style_div, 4);
        lv_style_set_text_font(&style_div, &font_roboto_regular_18);
        lv_style_set_border_width(&style_div, 0);
        lv_style_set_bg_color(&style_div, COLOR_BG_TITLEBAR);
        lv_style_set_text_color(&style_div, COLOR_WHITE);
        lv_style_set_flex_flow(&style_div, LV_FLEX_FLOW_ROW);
        lv_style_set_layout(&style_div, LV_LAYOUT_FLEX);
        lv_style_set_flex_cross_place(&style_div, LV_FLEX_ALIGN_CENTER);

        lv_style_init(&style_pressed);
        lv_style_set_recolor_opa(&style_pressed, LV_OPA_80);
        lv_style_set_recolor(&style_pressed, COLOR_BG_TITLEBAR);

        lv_style_init(&style_box);
        lv_style_set_bg_color(&style_box, COLOR_BG_TITLEBAR);
        lv_style_set_border_width(&style_box, 0);
        lv_style_set_width(&style_box, lv_pct(100));
        lv_style_set_radius(&style_box, 0);
        lv_style_set_height(&style_box, lv_pct(50));
        lv_style_set_align(&style_box, LV_ALIGN_BOTTOM_MID);

        lv_style_init(&style_icon);
        lv_style_set_recolor(&style_icon, COLOR_WHITE);
        lv_style_set_recolor_opa(&style_icon, LV_OPA_COVER);
        // lv_style_set_border_color(&style_icon, TEXT_ON_SURFACE_PRIMARY_LIGHT);
        // lv_style_set_border_width(&style_icon, 1);

        style_inited = true;
    }

    lv_obj_t *lv_button_0 = lv_button_create(parent);
    lv_obj_set_name_static(lv_button_0, "button_#");

    lv_obj_remove_style_all(lv_button_0);
    lv_obj_add_style(lv_button_0, &style_base, 0);
    lv_obj_add_style(lv_button_0, &style_pressed, LV_STATE_PRESSED);
    // lv_obj_bind_style(lv_button_0, &style_dark, 0, &dark_theme, 1);
    // lv_obj_bind_style(lv_button_0, &style_pressed_dark, LV_STATE_PRESSED, &dark_theme, 1);
    lv_obj_t *lv_obj_0 = lv_obj_create(lv_button_0);
    lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_add_style(lv_obj_0, &style_box, 0);

    lv_obj_t *lv_obj_1 = lv_obj_create(lv_button_0);
    lv_obj_set_flag(lv_obj_1, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_set_align(lv_obj_1, LV_ALIGN_CENTER);
    lv_obj_set_flag(lv_obj_1, LV_OBJ_FLAG_EVENT_BUBBLE, true);
    lv_obj_add_style(lv_obj_1, &style_div, 0);

    if (icon != NULL)
    {
        lv_obj_t *lv_image_0 = lv_image_create(lv_obj_1);
        lv_image_set_src(lv_image_0, icon);
        lv_obj_set_align(lv_image_0, LV_ALIGN_LEFT_MID);
        lv_obj_add_style(lv_image_0, &style_icon, 0);
    }
    lv_obj_t *lv_label_0 = lv_label_create(lv_obj_1);
    lv_label_set_text(lv_label_0, label);
    lv_obj_set_align(lv_label_0, LV_ALIGN_RIGHT_MID);
    lv_obj_set_style_pad_top(lv_label_0, 2, 0);

    // lv_obj_set_style_border_width(lv_label_0, 1, 0);
    // lv_obj_set_style_border_color(lv_label_0, TEXT_ON_SURFACE_PRIMARY_LIGHT, 0);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_button_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
