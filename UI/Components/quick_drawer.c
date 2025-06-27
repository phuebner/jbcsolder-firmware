#include "quick_drawer.h"
#include "theme.h"

LV_FONT_DECLARE(lv_font_roboto_24);

static const lv_coord_t DRAWER_WIDTH = 63;
static const lv_coord_t DRAWER_PADDING_VER = 14;

static lv_style_t sty_quick_drawer;
static lv_style_t sty_btn_quick_drawer;
static lv_style_t sty_btn_quick_drawer_pressed;
static lv_style_t sty_separator;

static void anim_x_cb(void *var, int32_t v);
static void anim_x_hide_cb(void *var, int32_t v);

static void setup_styles()
{
    // Cascade styles for quick drawer
    // Quick drawer background (main style)
    lv_style_init(&sty_quick_drawer);
    lv_style_set_bg_opa(&sty_quick_drawer, LV_OPA_90);
    lv_style_set_bg_color(&sty_quick_drawer, COLOR_BG_PRESET_DRAWER);
    lv_style_set_radius(&sty_quick_drawer, 10);
    lv_style_set_border_width(&sty_quick_drawer, 0);
    lv_style_set_pad_all(&sty_quick_drawer, 0);

    // Quick drawer shadow
    lv_style_set_shadow_width(&sty_quick_drawer, 15);
    lv_style_set_shadow_spread(&sty_quick_drawer, 1);
    lv_style_set_shadow_offset_x(&sty_quick_drawer, -2);
    lv_style_set_shadow_offset_y(&sty_quick_drawer, 2);
    lv_style_set_shadow_opa(&sty_quick_drawer, LV_OPA_30);

    // Text style for buttons
    lv_style_set_text_color(&sty_quick_drawer, lv_color_white());
    lv_style_set_text_font(&sty_quick_drawer, &lv_font_roboto_24);

    // Button style (on top of quick drawer main style)
    lv_style_init(&sty_btn_quick_drawer);
    lv_style_set_bg_opa(&sty_btn_quick_drawer, LV_OPA_TRANSP);
    lv_style_set_shadow_width(&sty_btn_quick_drawer, 0);
    lv_style_set_shadow_spread(&sty_btn_quick_drawer, 0);

    // Pressed style for buttons (on top of quick drawer main style and button style)
    lv_style_init(&sty_btn_quick_drawer_pressed);
    lv_style_set_bg_color(&sty_btn_quick_drawer_pressed, lv_color_black());
    lv_style_set_bg_opa(&sty_btn_quick_drawer_pressed, LV_OPA_30);

    // Separator style
    lv_style_init(&sty_separator);
    lv_style_set_line_width(&sty_separator, 1);
    lv_style_set_line_color(&sty_separator, COLOR_PRESET_SEPARATOR);
    lv_style_set_line_rounded(&sty_separator, false);
}

static lv_obj_t *quick_drawer_button_create(lv_obj_t *parent, const char *text)
{
    lv_obj_t *btn_quick_drawer = lv_button_create(parent);
    lv_obj_add_style(btn_quick_drawer, &sty_quick_drawer, 0);
    lv_obj_add_style(btn_quick_drawer, &sty_btn_quick_drawer, 0);
    lv_obj_add_style(btn_quick_drawer, &sty_btn_quick_drawer_pressed, LV_STATE_PRESSED);
    const int32_t width = lv_obj_get_width(parent);
    const int32_t height = lv_obj_get_height(parent) / 3;
    const lv_coord_t corner_radius = lv_obj_get_style_radius(btn_quick_drawer, LV_PART_MAIN);
    lv_obj_set_size(btn_quick_drawer, width, height);

    lv_obj_t *label = lv_label_create(btn_quick_drawer);
    lv_label_set_text(label, text);
    lv_obj_align(label, LV_ALIGN_CENTER, -corner_radius / 2, 0);
    return btn_quick_drawer;
}

static lv_obj_t *quick_drawer_separator_create(lv_obj_t *parent)
{
    lv_obj_t *separator = lv_line_create(parent);
    static lv_point_precise_t line_points[] = {{0, 0}, {0, 0}}; // Must be static because otherwise line will not be drawn
    line_points[1].x = lv_obj_get_width(parent) - 8;            // Set the width of the separator line
    lv_line_set_points(separator, line_points, 2);              // Set the points for the line
    lv_obj_add_style(separator, &sty_separator, 0);             // Apply the separator style
    return separator;
}

lv_obj_t *quick_drawer_create(lv_obj_t *parent)
{
    lv_obj_t *quick_drawer;

    setup_styles();

    const lv_coord_t drawer_height = (lv_obj_get_height(lv_scr_act()) - (2 * DRAWER_PADDING_VER));

    // Preset drawer create and apply style
    quick_drawer = lv_obj_create(lv_scr_act());
    lv_obj_add_style(quick_drawer, &sty_quick_drawer, 0);

    // Set size and position
    const lv_coord_t corner_radius = lv_obj_get_style_radius(quick_drawer, LV_PART_MAIN);
    lv_obj_set_size(quick_drawer, DRAWER_WIDTH + corner_radius, drawer_height); // Add corner radius to width to hide right corner
    lv_obj_align(quick_drawer, LV_ALIGN_RIGHT_MID, corner_radius, 0);

    lv_obj_update_layout(parent); // This is needed to get the correct width and height of the object

    /* --------------------------------- Buttons -------------------------------- */
    lv_obj_t *btn_quick1 = quick_drawer_button_create(quick_drawer, "350");
    lv_obj_align(btn_quick1, LV_ALIGN_TOP_LEFT, 0, 0);
    // lv_obj_set_event_cb(btn_quick1, btn_quick_event_cb);

    lv_obj_t *btn_quick2 = quick_drawer_button_create(quick_drawer, "300");
    lv_obj_align(btn_quick2, LV_ALIGN_LEFT_MID, 0, 0);
    // lv_obj_set_event_cb(btn_quick2, btn_quick_event_cb);

    lv_obj_t *btn_quick3 = quick_drawer_button_create(quick_drawer, "250");
    lv_obj_align(btn_quick3, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    // lv_obj_set_event_cb(btn_quick3, btn_quick_event_cb);

    /* -------------------------------- Separator ------------------------------- */
    lv_obj_t *line1 = quick_drawer_separator_create(quick_drawer);
    lv_obj_align(line1, LV_ALIGN_CENTER, 0, (drawer_height / 6));

    lv_obj_t *line2 = quick_drawer_separator_create(quick_drawer);
    lv_obj_align(line2, LV_ALIGN_CENTER, 0, -(drawer_height / 6));

    lv_obj_update_layout(parent); // This is needed to get the correct width and height of the object

    return quick_drawer;
}

static void anim_x_cb(void *var, int32_t v)
{
    lv_obj_set_x(var, v);
}

lv_anim_t *quick_drawer_show(lv_obj_t *obj)
{
    lv_anim_t *running_anim;
    // if (running_anim)
    // {
    //     lv_anim_del(obj, NULL); // Stop any running animation
    // }

    const lv_coord_t corner_radius = lv_obj_get_style_radius(obj, LV_PART_MAIN);

    static lv_anim_t animation_fly_in;
    lv_anim_init(&animation_fly_in);
    lv_anim_set_exec_cb(&animation_fly_in, (lv_anim_exec_xcb_t)anim_x_cb);
    lv_anim_set_var(&animation_fly_in, obj);
    lv_anim_set_duration(&animation_fly_in, ANIMATION_TIME);
    lv_anim_set_values(&animation_fly_in, lv_obj_get_width(obj), corner_radius); // Move in from the right until only the corner radius is hidden
    running_anim = lv_anim_start(&animation_fly_in);
    return running_anim;
}

lv_anim_t *quick_drawer_hide(lv_obj_t *obj)
{
    lv_anim_t *running_anim;
    // if (running_anim)
    // {
    //     lv_anim_del(obj, NULL); // Stop any running animation
    // }

    const lv_coord_t corner_radius = lv_obj_get_style_radius(obj, LV_PART_MAIN);

    lv_anim_t animation_fly_out;
    lv_anim_init(&animation_fly_out);
    lv_anim_set_exec_cb(&animation_fly_out, (lv_anim_exec_xcb_t)anim_x_cb);
    lv_anim_set_var(&animation_fly_out, obj);
    lv_anim_set_duration(&animation_fly_out, ANIMATION_TIME);
    lv_anim_set_values(&animation_fly_out, corner_radius, lv_obj_get_width(obj)); // Move out to the right
    running_anim = lv_anim_start(&animation_fly_out);
    return running_anim;
}
