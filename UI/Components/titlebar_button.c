#include "titlebar_button.h"
#include "theme.h"

static const lv_coord_t TITLEBAR_BUTTON_HEIGHT = 55;

static lv_style_t sty_btn_titlebar;

static void setup_styles()
{
    lv_style_init(&sty_btn_titlebar);
    lv_style_set_bg_opa(&sty_btn_titlebar, LV_OPA_COVER);
    lv_style_set_bg_color(&sty_btn_titlebar, COLOR_BG_TITLEBAR);
    lv_style_set_border_width(&sty_btn_titlebar, 0);
    lv_style_set_shadow_width(&sty_btn_titlebar, 0);
    lv_style_set_radius(&sty_btn_titlebar, 20);
    lv_style_set_text_color(&sty_btn_titlebar, COLOR_WHITE);
    lv_style_set_text_font(&sty_btn_titlebar, &lv_font_montserrat_28);
}

lv_obj_t *titlebar_button_create(lv_obj_t *parent)
{
    setup_styles();

    lv_obj_t *btn_titlebar = lv_button_create(lv_screen_active());
    lv_obj_add_style(btn_titlebar, &sty_btn_titlebar, 0);

    const lv_coord_t _corner_radius = lv_obj_get_style_radius(btn_titlebar, LV_PART_MAIN); // Get corner radius of button from style
    // const lv_coord_t _button_width = CENTER_AREA_POSITION.x + 40;
    const lv_coord_t _button_width = 30 + 40; // Button should start from the left edge and reach into the center area

    lv_obj_set_size(btn_titlebar, _button_width + _corner_radius, TITLEBAR_BUTTON_HEIGHT);
    lv_obj_set_pos(btn_titlebar, -_corner_radius, 0);

    lv_obj_remove_flag(btn_titlebar, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    lv_obj_set_layout(btn_titlebar, LV_LAYOUT_NONE); // This allows us to align the label of the button manually
    lv_obj_t *lbl_btn_menu = lv_label_create(btn_titlebar);
    lv_label_set_text(lbl_btn_menu, LV_SYMBOL_BARS);
    lv_obj_align(lbl_btn_menu, LV_ALIGN_CENTER, (_corner_radius / 2), 0);
    lv_obj_set_ext_click_area(btn_titlebar, 20);

    return btn_titlebar;
}