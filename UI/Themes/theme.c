// /**
//  * @file theme.c
//  * @author Patrick Huebner (phuebner86@gmail.com)
//  * @brief GUI Theme
//  * @version 0.1
//  * @date 2021-01-06
//  *
//  * @copyright Copyright (c) 2021
//  *
//  */

// /* -------------------------------------------------------------------------- */
// /*                                   INCLUDE                                  */
// /* -------------------------------------------------------------------------- */

#include "theme.h"
#include "lvgl.h"

// /* -------------------------------------------------------------------------- */
// /*                              STATIC PROTOTYPES                             */
// /* -------------------------------------------------------------------------- */

// static void styles_init(void);
// static void theme_apply_cb(lv_theme_t *th, lv_obj_t *obj, lv_theme_style_t name);

// /* -------------------------------------------------------------------------- */
// /*                              STATIC VARIABLES                              */
// /* -------------------------------------------------------------------------- */
// static lv_theme_t custom_theme;
// static lv_style_t sty_title_bg;
// static lv_style_t sty_titlebar_btn;
// static lv_style_t sty_preset_drawer_bg;
// static lv_style_t sty_preset_btn;
// static lv_style_t sty_preset_separator;
// static lv_style_t sty_power_bar_bg;
// static lv_style_t sty_power_bar_indicator;
// static lv_style_t sty_sidebar_btn;

// /* -------------------------------------------------------------------------- */
// /*                              GLOBAL FUNCTIONS                              */
// /* -------------------------------------------------------------------------- */

void theme_init()
{
    lv_display_t *display = lv_disp_get_default();
    lv_theme_t *th = lv_theme_default_init(display,       // Use DPI, size, etc. from this display
                                           COLOR_PRIMARY, // Primary and secondary palette colors
                                           COLOR_SECONDARY,
                                           false, // Dark theme?  False = light theme.
                                           &lv_font_montserrat_14);

    lv_display_set_theme(display, th); /* Assign theme to display */

    /*Initialize styles for the new theme*/
    // styles_init();

    // /* Get the current theme (e.g. material). It will be the base of the custom theme. */
    // lv_theme_t *default_theme = lv_theme_get_act();

    // /* Initialize a custom theme */
    // lv_theme_copy(&custom_theme, default_theme);          // Initialize the custom theme from the base theme
    // lv_theme_set_apply_cb(&custom_theme, theme_apply_cb); // Set a custom theme apply callback
    // lv_theme_set_base(&custom_theme, default_theme);      // Set the base theme of the csutom theme

    // custom_theme.color_primary = COLOR_PRIMARY;
    // custom_theme.color_secondary = color_secondary;
    // custom_theme.font_small = font_small;
    // custom_theme.font_normal = font_normal;
    // custom_theme.font_subtitle = font_subtitle;
    // custom_theme.font_title = font_title;
    // custom_theme.flags = flags;

    // return &custom_theme;
}

// /* -------------------------------------------------------------------------- */
// /*                              STATIC FUNCTIONS                              */
// /* -------------------------------------------------------------------------- */

// static void styles_init(void)
// {

//     /* Power bar bg*/
//     lv_style_init(&sty_power_bar_bg);
//     lv_style_set_radius(&sty_power_bar_bg, LV_STATE_DEFAULT, 4);

//     /* Power bar inidcator */
//     lv_style_init(&sty_power_bar_indicator);
//     lv_style_set_radius(&sty_power_bar_indicator, LV_STATE_DEFAULT, 4);
//     lv_style_set_bg_color(&sty_power_bar_indicator, LV_STATE_DEFAULT,
//                           COLOR_PRIMARY);

//     /* Sidebar button */
//     lv_style_init(&sty_sidebar_btn);
//     lv_style_set_bg_opa(&sty_sidebar_btn, LV_STATE_DEFAULT, LV_OPA_TRANSP);
//     lv_style_set_text_color(&sty_sidebar_btn, LV_STATE_DEFAULT, COLOR_WHITE);
//     lv_style_set_value_color(&sty_sidebar_btn, LV_STATE_DEFAULT, COLOR_WHITE);
//     lv_style_set_text_font(&sty_sidebar_btn, LV_STATE_DEFAULT, &lv_font_symbols_28);
//     lv_style_set_radius(&sty_sidebar_btn, LV_STATE_DEFAULT, 0);
// }

// static void theme_apply_cb(lv_theme_t *th, lv_obj_t *obj, lv_theme_style_t name)
// {
//     lv_style_list_t *list;

//     /*To avoid warnings*/
//     uint32_t name_int = (uint32_t)name;
//     switch (name_int)
//     {
//     case CUSTOM_THEME_TITLE_BG:
//         lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
//         list = lv_obj_get_style_list(obj, LV_OBJ_PART_MAIN);
//         _lv_style_list_add_style(list, &sty_title_bg);
//         break;

//     case CUSTOM_THEME_TITLEBAR_BTN:
//         lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
//         list = lv_obj_get_style_list(obj, LV_OBJ_PART_MAIN);
//         _lv_style_list_add_style(list, &sty_titlebar_btn);
//         break;

//     case CUSTOM_THEME_PRESET_DRAWER:
//         lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
//         list = lv_obj_get_style_list(obj, LV_OBJ_PART_MAIN);
//         _lv_style_list_add_style(list, &sty_preset_drawer_bg);
//         break;

//     case CUSTOM_THEME_PRESET_BTN:
//         lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
//         list = lv_obj_get_style_list(obj, LV_OBJ_PART_MAIN);
//         _lv_style_list_add_style(list, &sty_preset_btn);
//         break;

//     case CUSTOM_THEME_PRESET_SEPARATOR:
//         lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
//         list = lv_obj_get_style_list(obj, LV_OBJ_PART_MAIN);
//         _lv_style_list_add_style(list, &sty_preset_separator);
//         break;

//     case CUSTOM_THEME_POWER_BAR:
//         // lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
//         list = lv_obj_get_style_list(obj, LV_BAR_PART_BG);
//         _lv_style_list_add_style(list, &sty_power_bar_bg);
//         list = lv_obj_get_style_list(obj, LV_BAR_PART_INDIC);
//         _lv_style_list_add_style(list, &sty_power_bar_indicator);
//         break;

//     case CUSTOM_THEME_SIDEBAR_BTN:
//         lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
//         list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
//         _lv_style_list_add_style(list, &sty_sidebar_btn);
//         break;

//     default:
//         break;
//     }

//     // switch(name_int) {
//     //     case LV_THEME_NONE:
//     //         break;

//     //     case LV_THEME_SCR:
//     //         lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
//     //         list = lv_obj_get_style_list(obj, LV_OBJ_PART_MAIN);
//     //         _lv_style_list_add_style(list, &style_bg);
//     //         break;

//     //     case LV_THEME_OBJ:
//     //         lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
//     //         list = lv_obj_get_style_list(obj, LV_OBJ_PART_MAIN);
//     //         _lv_style_list_add_style(list, &style_box);
//     //         break;

//     //     case LV_DEMO_PRINTER_THEME_BOX_BORDER:
//     //         lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
//     //         list = lv_obj_get_style_list(obj, LV_OBJ_PART_MAIN);
//     //         _lv_style_list_add_style(list, &style_box);
//     //         _lv_style_list_add_style(list, &style_box_border);
//     //         break;

//     //     case LV_THEME_CONT:
//     //         lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
//     //         list = lv_obj_get_style_list(obj, LV_CONT_PART_MAIN);
//     //         _lv_style_list_add_style(list, &style_box);
//     //         break;

//     //     case LV_DEMO_PRINTER_THEME_BTN_CIRCLE:
//     //         lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
//     //         list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
//     //         _lv_style_list_add_style(list, &style_btn);
//     //         _lv_style_list_add_style(list, &style_circle);
//     //         break;

//     //     case LV_DEMO_PRINTER_THEME_BTN_BORDER:
//     //         lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
//     //         list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
//     //         _lv_style_list_add_style(list, &style_btn);
//     //         _lv_style_list_add_style(list, &style_btn_border);
//     //         break;

//     //     case LV_DEMO_PRINTER_THEME_BTN_BACK:
//     //         lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
//     //         list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
//     //         _lv_style_list_add_style(list, &style_back);
//     //         break;

//     //     case LV_THEME_BTN:
//     //         lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
//     //         list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
//     //         _lv_style_list_add_style(list, &style_btn);
//     //         break;

//     //     case LV_THEME_BAR:
//     //         lv_obj_clean_style_list(obj, LV_BAR_PART_BG);
//     //         list = lv_obj_get_style_list(obj, LV_BAR_PART_BG);

//     //         lv_obj_clean_style_list(obj, LV_BAR_PART_INDIC);
//     //         list = lv_obj_get_style_list(obj, LV_BAR_PART_INDIC);
//     //         _lv_style_list_add_style(list, &style_bar_indic);
//     //         break;

//     //     case LV_THEME_IMAGE:
//     //         lv_obj_clean_style_list(obj, LV_IMG_PART_MAIN);
//     //         list = lv_obj_get_style_list(obj, LV_IMG_PART_MAIN);
//     //         break;

//     //     case LV_DEMO_PRINTER_THEME_ICON:
//     //         lv_obj_clean_style_list(obj, LV_IMG_PART_MAIN);
//     //         list = lv_obj_get_style_list(obj, LV_IMG_PART_MAIN);
//     //         _lv_style_list_add_style(list, &style_icon);
//     //         break;

//     //     case LV_THEME_LABEL:
//     //         lv_obj_clean_style_list(obj, LV_LABEL_PART_MAIN);
//     //         list = lv_obj_get_style_list(obj, LV_LABEL_PART_MAIN);
//     //         break;

//     //     case LV_DEMO_PRINTER_THEME_TITLE:
//     //         lv_obj_clean_style_list(obj, LV_LABEL_PART_MAIN);
//     //         list = lv_obj_get_style_list(obj, LV_LABEL_PART_MAIN);
//     //         _lv_style_list_add_style(list, &style_title);
//     //         break;

//     //     case LV_DEMO_PRINTER_THEME_LABEL_WHITE:
//     //         lv_obj_clean_style_list(obj, LV_LABEL_PART_MAIN);
//     //         list = lv_obj_get_style_list(obj, LV_LABEL_PART_MAIN);
//     //         _lv_style_list_add_style(list, &style_label_white);
//     //         break;

//     //     case LV_THEME_SLIDER:
//     //         lv_obj_clean_style_list(obj, LV_SLIDER_PART_BG);
//     //         list = lv_obj_get_style_list(obj, LV_SLIDER_PART_BG);
//     //         _lv_style_list_add_style(list, &style_sw_bg);

//     //         lv_obj_clean_style_list(obj, LV_SLIDER_PART_INDIC);
//     //         list = lv_obj_get_style_list(obj, LV_SLIDER_PART_INDIC);

//     //         lv_obj_clean_style_list(obj, LV_SLIDER_PART_KNOB);
//     //         list = lv_obj_get_style_list(obj, LV_SLIDER_PART_KNOB);
//     //         _lv_style_list_add_style(list, &style_slider_knob);
//     //         break;

//     //     case LV_THEME_LIST:
//     //         lv_obj_clean_style_list(obj, LV_LIST_PART_BG);
//     //         list = lv_obj_get_style_list(obj, LV_LIST_PART_BG);
//     //         _lv_style_list_add_style(list, &style_box);

//     //         lv_obj_clean_style_list(obj, LV_LIST_PART_SCROLLABLE);
//     //         list = lv_obj_get_style_list(obj, LV_LIST_PART_SCROLLABLE);

//     //         lv_obj_clean_style_list(obj, LV_LIST_PART_SCROLLBAR);
//     //         list = lv_obj_get_style_list(obj, LV_LIST_PART_SCROLLBAR);
//     //         _lv_style_list_add_style(list, &style_scrollbar);
//     //         break;

//     //     case LV_THEME_LIST_BTN:
//     //         lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
//     //         list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
//     //         _lv_style_list_add_style(list, &style_list_btn);
//     //         break;

//     //     case LV_THEME_ARC:
//     //         lv_obj_clean_style_list(obj, LV_ARC_PART_BG);
//     //         list = lv_obj_get_style_list(obj, LV_ARC_PART_BG);
//     //         _lv_style_list_add_style(list, &style_arc_bg);

//     //         lv_obj_clean_style_list(obj, LV_ARC_PART_INDIC);
//     //         list = lv_obj_get_style_list(obj, LV_ARC_PART_INDIC);
//     //         _lv_style_list_add_style(list, &style_arc_indic);
//     //         break;

//     //     case LV_THEME_SWITCH:
//     //         lv_obj_clean_style_list(obj, LV_SWITCH_PART_BG);
//     //         list = lv_obj_get_style_list(obj, LV_SWITCH_PART_BG);
//     //         _lv_style_list_add_style(list, &style_sw_bg);

//     //         lv_obj_clean_style_list(obj, LV_SWITCH_PART_INDIC);
//     //         list = lv_obj_get_style_list(obj, LV_SWITCH_PART_INDIC);
//     //         _lv_style_list_add_style(list, &style_sw_indic);

//     //         lv_obj_clean_style_list(obj, LV_SWITCH_PART_KNOB);
//     //         list = lv_obj_get_style_list(obj, LV_SWITCH_PART_KNOB);
//     //         _lv_style_list_add_style(list, &style_sw_knob);
//     //         break;

//     //     case LV_THEME_DROPDOWN:
//     //         lv_obj_clean_style_list(obj, LV_DROPDOWN_PART_MAIN);
//     //         list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_MAIN);
//     //         _lv_style_list_add_style(list, &style_btn);
//     //         _lv_style_list_add_style(list, &style_pad);

//     //         lv_obj_clean_style_list(obj, LV_DROPDOWN_PART_LIST);
//     //         list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_LIST);
//     //         _lv_style_list_add_style(list, &style_box);
//     //         _lv_style_list_add_style(list, &style_ddlist_list);
//     //         _lv_style_list_add_style(list, &style_pad);

//     //         lv_obj_clean_style_list(obj, LV_DROPDOWN_PART_SELECTED);
//     //         list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_SELECTED);
//     //         _lv_style_list_add_style(list, &style_ddlist_selected);

//     //         lv_obj_clean_style_list(obj, LV_DROPDOWN_PART_SCROLLBAR);
//     //         list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_SCROLLBAR);
//     //         _lv_style_list_add_style(list, &style_scrollbar);
//     //         break;

//     //     default:
//     //         break;
//     // }

//     lv_obj_refresh_style(obj, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);
// }
