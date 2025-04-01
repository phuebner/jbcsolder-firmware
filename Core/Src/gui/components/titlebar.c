#include "titlebar.h"

static const lv_coord_t TITLEBAR_HEIGHT = 30;

static lv_style_t sty_titlebar;

static void setup_styles()
{
    lv_style_init(&sty_titlebar);
    lv_style_set_bg_opa(&sty_titlebar, LV_OPA_COVER);
    lv_style_set_bg_color(&sty_titlebar, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_border_width(&sty_titlebar, 0);
    lv_style_set_shadow_width(&sty_titlebar, 0);
    lv_style_set_radius(&sty_titlebar, 0);
    lv_style_set_text_color(&sty_titlebar, lv_color_white());
}

lv_obj_t *titlebar_create(lv_obj_t *parent)
{
    setup_styles();

    lv_obj_t *titlebar = lv_btn_create(lv_scr_act());
    lv_obj_add_style(titlebar, &sty_titlebar, 0);

    const lv_coord_t _corner_radius = lv_obj_get_style_radius(titlebar, LV_PART_MAIN); // Get corner radius of button from style
    // const lv_coord_t _button_width = CENTER_AREA_POSITION.x + 40;
    const lv_coord_t _button_width = 30 + 40; // Button should start from the left edge and reach into the center area

    lv_obj_set_size(titlebar, lv_obj_get_width(parent), TITLEBAR_HEIGHT);
    lv_obj_set_pos(titlebar, 0, 0);

    return titlebar;
}