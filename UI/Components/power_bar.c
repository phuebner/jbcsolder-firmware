#include "power_bar.h"
#include "theme.h"

static const lv_coord_t POWER_BAR_WIDTH = 15;
// static const lv_coord_t POWER_BAR_PADDING_HOR = 10;
static const lv_coord_t POWER_BAR_PADDING_VER = 14;

static lv_style_t sty_power_bar_bg;
static lv_style_t sty_power_bar_indicator;

static void setup_styles()
{
    // Power bar background
    lv_style_init(&sty_power_bar_bg);
    lv_style_set_bg_color(&sty_power_bar_bg, COLOR_BG_PRESET_DRAWER);
    lv_style_set_bg_opa(&sty_power_bar_bg, LV_OPA_COVER);
    lv_style_set_border_width(&sty_power_bar_bg, 0);
    lv_style_set_radius(&sty_power_bar_bg, 4);
    lv_style_set_pad_all(&sty_power_bar_bg, 0);
    lv_style_set_margin_bottom(&sty_power_bar_bg, 5);
    lv_style_set_margin_top(&sty_power_bar_bg, 5);
    lv_style_set_margin_left(&sty_power_bar_bg, 5);
    lv_style_set_margin_right(&sty_power_bar_bg, 5);

    // Power bar indicator
    lv_style_init(&sty_power_bar_indicator);
    lv_style_set_bg_color(&sty_power_bar_indicator, COLOR_PRIMARY);
    lv_style_set_bg_opa(&sty_power_bar_indicator, LV_OPA_COVER);
    lv_style_set_radius(&sty_power_bar_bg, 4);
}

lv_obj_t *power_bar_create(lv_obj_t *parent)
{
    setup_styles();

    lv_obj_t *bar_power = lv_bar_create(parent);
    lv_obj_add_style(bar_power, &sty_power_bar_bg, 0);
    lv_obj_add_style(bar_power, &sty_power_bar_indicator, LV_PART_INDICATOR);

    lv_obj_set_size(bar_power, POWER_BAR_WIDTH, lv_pct(100));

    lv_bar_set_range(bar_power, 0, 20);
    lv_bar_set_value(bar_power, 0, LV_ANIM_ON);
    return bar_power;
}

void power_bar_set_value(lv_obj_t *obj, int32_t value)
{
    lv_bar_set_value(obj, value, LV_ANIM_ON);
}

lv_observer_t *power_bar_bind_power(lv_obj_t *obj, lv_subject_t *subject)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_NULL(subject);

    lv_bar_bind_value(obj, subject);
}
