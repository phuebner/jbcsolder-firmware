#ifndef __THEME_H__
#define __THEME_H__

#ifdef __cplusplus
extern "C"
{
#endif

    /* -------------------------------------------------------------------------- */
    /*                                  INCLUDES                                  */
    /* -------------------------------------------------------------------------- */

#include "lvgl.h"

    /* -------------------------------------------------------------------------- */
    /*                                   DEINES                                   */
    /* -------------------------------------------------------------------------- */

#define COLOR_PRIMARY lv_color_hex(0xA00101)
#define COLOR_SECONDARY lv_color_hex(0x1295b9)

#define COLOR_WHITE lv_color_hex(0xFFFFFF)

#define COLOR_BG_PRESET_DRAWER lv_color_hex(0x4D4D4D)
#define COLOR_PRESET_SEPARATOR lv_color_hex(0x8F8F8F)

#define COLOR_BG_TITLEBAR lv_color_hex(0xA00101)      // dark red
#define COLOR_BG_PRESET_DRAWER lv_color_hex(0x4D4D4D) // dark grey

#define ANIMATION_TIME 200

    // #define LV_SYMBOL_BARS "\xef\x83\x89"
    /* -------------------------------------------------------------------------- */
    /*                                  TYPEDEFS                                  */
    /* -------------------------------------------------------------------------- */

    // typedef enum
    // {
    //     CUSTOM_THEME_TITLE_BG = LV_THEME_CUSTOM_START,
    //     CUSTOM_THEME_TITLEBAR_BTN,
    //     CUSTOM_THEME_PRESET_DRAWER,
    //     CUSTOM_THEME_PRESET_BTN,
    //     CUSTOM_THEME_PRESET_SEPARATOR,
    //     CUSTOM_THEME_POWER_BAR,
    //     CUSTOM_THEME_SIDEBAR_BTN
    // } custom_theme_style_t;

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
    void theme_init();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __THEME_H__ */
