#include "titlebar.h"

static const lv_coord_t TITLEBAR_HEIGHT = 35;

static lv_style_t sty_titlebar;
static lv_style_t sty_titlebar_button_checked;

static void setup_styles()
{
    lv_style_init(&sty_titlebar);
    lv_style_set_bg_opa(&sty_titlebar, LV_OPA_COVER);
    lv_style_set_bg_color(&sty_titlebar, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_border_width(&sty_titlebar, 0);
    lv_style_set_shadow_width(&sty_titlebar, 0);
    lv_style_set_radius(&sty_titlebar, 0);
    lv_style_set_text_color(&sty_titlebar, lv_color_white());
    lv_style_set_text_font(&sty_titlebar, &lv_font_montserrat_24);
    lv_style_set_pad_all(&sty_titlebar, 0);

    lv_style_init(&sty_titlebar_button_checked);
    lv_style_set_bg_color(&sty_titlebar_button_checked, lv_palette_main(LV_PALETTE_AMBER));
}

lv_obj_t *titlebar_create(lv_obj_t *parent)
{
    setup_styles();

    lv_obj_t *titlebar = lv_obj_create(lv_screen_active());
    lv_obj_add_style(titlebar, &sty_titlebar, 0);
    lv_obj_remove_flag(titlebar, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_size(titlebar, lv_obj_get_width(parent), TITLEBAR_HEIGHT);
    lv_obj_set_pos(titlebar, 0, 0);

#ifdef SIMULATOR
    /* Button only available in the simulator, to toggle between in-stand and out of stand */
    lv_obj_t *btn_sim_toggle_stand = lv_button_create(titlebar);
    lv_obj_add_style(btn_sim_toggle_stand, &sty_titlebar, LV_STATE_DEFAULT);
    lv_obj_add_style(btn_sim_toggle_stand, &sty_titlebar_button_checked, LV_STATE_CHECKED);
    lv_obj_set_size(btn_sim_toggle_stand, 50, TITLEBAR_HEIGHT);
    lv_obj_set_pos(btn_sim_toggle_stand, 80, 0);
    lv_obj_add_flag(btn_sim_toggle_stand, LV_OBJ_FLAG_CHECKABLE);

    lv_obj_t *lbl_btn_sim_toggle_stand = lv_label_create(btn_sim_toggle_stand);
    lv_obj_align(lbl_btn_sim_toggle_stand, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(lbl_btn_sim_toggle_stand, LV_SYMBOL_LOOP);
#endif
    return titlebar;
}