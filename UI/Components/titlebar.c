#include "titlebar.h"
#include "iron.h"
#include "theme.h"

static const lv_coord_t TITLEBAR_HEIGHT = 35;

static void switch_to_stand_event_cb(lv_event_t *e);

extern iron_t iron_a;
static lv_style_t sty_titlebar;
static lv_style_t sty_titlebar_button_checked;

static void setup_styles()
{
    lv_style_init(&sty_titlebar);
    lv_style_set_bg_opa(&sty_titlebar, LV_OPA_COVER);
    lv_style_set_bg_color(&sty_titlebar, COLOR_BG_TITLEBAR);
    lv_style_set_border_width(&sty_titlebar, 0);
    lv_style_set_shadow_width(&sty_titlebar, 0);
    lv_style_set_radius(&sty_titlebar, 0);
    lv_style_set_text_color(&sty_titlebar, lv_color_white());
    lv_style_set_text_font(&sty_titlebar, &font_roboto_regular_24);
    lv_style_set_pad_all(&sty_titlebar, 0);

    lv_style_init(&sty_titlebar_button_checked);
    lv_style_set_bg_color(&sty_titlebar_button_checked, COLOR_BG_TITLEBAR);
}

lv_obj_t *titlebar_create(lv_obj_t *parent)
{
    setup_styles();

    lv_obj_t *titlebar = lv_obj_create(lv_screen_active());
    lv_obj_add_style(titlebar, &sty_titlebar, 0);
    lv_obj_remove_flag(titlebar, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_size(titlebar, lv_obj_get_width(parent), TITLEBAR_HEIGHT);
    lv_obj_set_pos(titlebar, 0, 0);

    return titlebar;
}
