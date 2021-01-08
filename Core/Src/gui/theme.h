/**
 * @file theme.h
 * @author Patrick Huebner (phuebner86@gmail.com)
 * @brief GUI Theme
 * @version 0.1
 * @date 2021-01-06
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __THEME_H__
#define __THEME_H__

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */
/*                                  INCLUDES                                  */
/* -------------------------------------------------------------------------- */

#include "lvgl/lvgl.h"

/* -------------------------------------------------------------------------- */
/*                                   DEINES                                   */
/* -------------------------------------------------------------------------- */

#define COLOR_PRIMARY			    lv_color_hex(0xA00101)

#define COLOR_WHITE			        lv_color_hex(0xFFFFFF)

#define COLOR_BG_PRESET_DRAWER		lv_color_hex(0x4D4D4D)
#define COLOR_PRESET_SEPERATOR		lv_color_hex(0x8F8F8F)

// #define LV_DEMO_PRINTER_WHITE           lv_color_hex(0xffffff)
// #define LV_DEMO_PRINTER_LIGHT           lv_color_hex(0xf3f8fe)
// #define LV_DEMO_PRINTER_GRAY            lv_color_hex(0x8a8a8a)
// #define LV_DEMO_PRINTER_LIGHT_GRAY      lv_color_hex(0xc4c4c4)
// #define LV_DEMO_PRINTER_BLUE            lv_color_hex(0x2f3243) //006fb6
// #define LV_DEMO_PRINTER_GREEN           lv_color_hex(0x4cb242)
// #define LV_DEMO_PRINTER_RED             lv_color_hex(0xd51732)

/* -------------------------------------------------------------------------- */
/*                                  TYPEDEFS                                  */
/* -------------------------------------------------------------------------- */

typedef enum {
    CUSTOM_THEME_TITLE_BG = LV_THEME_CUSTOM_START,
    CUSTOM_THEME_TITLEBAR_BTN,
    CUSTOM_THEME_PRESET_DRAWER,
    CUSTOM_THEME_PRESET_BTN, 
    CUSTOM_THEME_PRESET_SEPARATOR, 
    CUSTOM_THEME_POWER_BAR
}custom_theme_style_t;

/* -------------------------------------------------------------------------- */
/*                              GLOBAL PROTOTYPES                             */
/* -------------------------------------------------------------------------- */

/**
 * Initialize the default
 * @param color_primary the primary color of the theme
 * @param color_secondary the secondary color for the theme
 * @param flags ORed flags starting with `LV_THEME_DEF_FLAG_...`
 * @param font_small pointer to a small font
 * @param font_normal pointer to a normal font
 * @param font_subtitle pointer to a large font
 * @param font_title pointer to a extra large font
 * @return a pointer to reference this theme later
 */
// lv_theme_t * lv_demo_printer_theme_init(lv_color_t color_primary, lv_color_t color_secondary, uint32_t flags,
//                                     const lv_font_t * font_small, const lv_font_t * font_normal, const lv_font_t * font_subtitle,
//                                     const lv_font_t * font_title);
lv_theme_t * theme_init();


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __THEME_H__ */

