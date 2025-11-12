#include "quick_drawer.h"
#include "theme.h"

static lv_style_t sty_quick_drawer;
static lv_style_t sty_btn_quick_drawer;
static lv_style_t sty_btn_quick_drawer_pressed;
static lv_style_t sty_separator;

typedef struct
{
    int btn_pressed;
} quick_drawer_user_data_t;

static void anim_x_cb(void *var, int32_t v);
static void anim_x_hide_cb(void *var, int32_t v);
static void btn_event_cb(lv_event_t *e);

static void setup_styles()
{
    // Cascade styles for quick drawer
    // Quick drawer background (main style)
    lv_style_init(&sty_quick_drawer);
    lv_style_set_bg_opa(&sty_quick_drawer, LV_OPA_90);
    lv_style_set_bg_color(&sty_quick_drawer, COLOR_BG_PRESET_DRAWER);
    lv_style_set_radius(&sty_quick_drawer, QUICK_DRAWER_RADIUS);
    lv_style_set_border_width(&sty_quick_drawer, 0);
    lv_style_set_pad_all(&sty_quick_drawer, 0);

    // Quick drawer shadow
    lv_style_set_shadow_width(&sty_quick_drawer, 15);
    lv_style_set_shadow_spread(&sty_quick_drawer, 1);
    lv_style_set_shadow_offset_x(&sty_quick_drawer, -2);
    lv_style_set_shadow_offset_y(&sty_quick_drawer, 2);
    lv_style_set_shadow_opa(&sty_quick_drawer, LV_OPA_30);

    // Button style (on top of quick drawer main style)
    lv_style_init(&sty_btn_quick_drawer);
    lv_style_set_bg_opa(&sty_btn_quick_drawer, LV_OPA_TRANSP);
    lv_style_set_radius(&sty_btn_quick_drawer, QUICK_DRAWER_RADIUS);
    lv_style_set_shadow_width(&sty_btn_quick_drawer, 0);
    lv_style_set_shadow_spread(&sty_btn_quick_drawer, 0);

    // Text style for buttons
    lv_style_set_text_color(&sty_btn_quick_drawer, lv_color_white());
    lv_style_set_text_font(&sty_btn_quick_drawer, &font_roboto_regular_24);

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
    lv_obj_add_style(btn_quick_drawer, &sty_btn_quick_drawer, 0);
    lv_obj_add_style(btn_quick_drawer, &sty_btn_quick_drawer_pressed, LV_STATE_PRESSED);
    const int32_t width = lv_obj_get_width(parent);
    const int32_t height = lv_obj_get_height(parent) / 3;
    const lv_coord_t corner_radius = lv_obj_get_style_radius(btn_quick_drawer, LV_PART_MAIN);
    lv_obj_set_size(btn_quick_drawer, lv_pct(100), height);

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

    quick_drawer_user_data_t *user_data = lv_malloc(sizeof(quick_drawer_user_data_t));
    user_data->btn_pressed = -1;

    const lv_coord_t drawer_height = (lv_obj_get_height(lv_scr_act()) - (2 * QUICK_DRAWER_PADDING_VER));

    // Preset drawer create and apply style
    quick_drawer = lv_obj_create(lv_scr_act());
    lv_obj_add_style(quick_drawer, &sty_quick_drawer, 0);

    // Set size and position
    const lv_coord_t corner_radius = lv_obj_get_style_radius(quick_drawer, LV_PART_MAIN);
    lv_obj_set_size(quick_drawer, QUICK_DRAWER_WIDTH + corner_radius, drawer_height); // Add corner radius to width to hide right corner
    lv_obj_align(quick_drawer, LV_ALIGN_RIGHT_MID, corner_radius, 0);

    lv_obj_update_layout(parent); // This is needed to get the correct width and height of the object
    lv_obj_set_user_data(quick_drawer, user_data);

    /* --------------------------------- Buttons -------------------------------- */
    lv_obj_t *btn_quick1 = quick_drawer_button_create(quick_drawer, "350");
    lv_obj_align(btn_quick1, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_add_event_cb(btn_quick1, btn_event_cb, LV_EVENT_ALL, 1);

    lv_obj_t *btn_quick2 = quick_drawer_button_create(quick_drawer, "300");
    lv_obj_align(btn_quick2, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_add_event_cb(btn_quick2, btn_event_cb, LV_EVENT_ALL, 2);

    lv_obj_t *btn_quick3 = quick_drawer_button_create(quick_drawer, "250");
    lv_obj_align(btn_quick3, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_add_event_cb(btn_quick3, btn_event_cb, LV_EVENT_ALL, 3);

    /* -------------------------------- Separator ------------------------------- */
    lv_obj_t *line1 = quick_drawer_separator_create(quick_drawer);
    lv_obj_align(line1, LV_ALIGN_CENTER, 0, (drawer_height / 6));

    lv_obj_t *line2 = quick_drawer_separator_create(quick_drawer);
    lv_obj_align(line2, LV_ALIGN_CENTER, 0, -(drawer_height / 6));

    lv_obj_update_layout(parent); // This is needed to get the correct width and height of the object

    return quick_drawer;
}

void quick_drawer_set_presets(lv_obj_t *obj, const int presets1, const int presets2, const int presets3)
{
    if (!obj)
        return;

    // Get buttons by their index in the quick drawer
    lv_obj_t *btn_quick1 = lv_obj_get_child(obj, 0);
    lv_obj_t *btn_quick2 = lv_obj_get_child(obj, 1);
    lv_obj_t *btn_quick3 = lv_obj_get_child(obj, 2);

    // Update button labels
    lv_obj_t *label1 = lv_obj_get_child(btn_quick1, 0);
    lv_label_set_text_fmt(label1, "%d", presets1);

    lv_obj_t *label2 = lv_obj_get_child(btn_quick2, 0);
    lv_label_set_text_fmt(label2, "%d", presets2);

    lv_obj_t *label3 = lv_obj_get_child(btn_quick3, 0);
    lv_label_set_text_fmt(label3, "%d", presets3);
}

static void anim_x_cb(void *var, int32_t v)
{
    lv_obj_set_x(var, v);
}

lv_anim_t *quick_drawer_show(lv_obj_t *obj)
{
    if (!obj)
        return NULL;

    if (quick_drawer_is_hidden(obj) == false)
        return NULL; // Already shown

    const lv_coord_t corner_radius = lv_obj_get_style_radius(obj, LV_PART_MAIN);

    static lv_anim_t animation_fly_in;
    lv_anim_init(&animation_fly_in);
    lv_anim_set_exec_cb(&animation_fly_in, (lv_anim_exec_xcb_t)anim_x_cb);
    lv_anim_set_var(&animation_fly_in, obj);
    lv_anim_set_duration(&animation_fly_in, ANIMATION_TIME);
    lv_anim_set_values(&animation_fly_in, lv_obj_get_width(obj), corner_radius); // Move in from the right until only the corner radius is hidden
    lv_anim_t *running_anim = lv_anim_start(&animation_fly_in);
    return running_anim;
}

lv_anim_t *quick_drawer_hide(lv_obj_t *obj)
{
    if (!obj)
        return NULL;

    if (quick_drawer_is_hidden(obj))
        return NULL; // Already hidden

    const lv_coord_t corner_radius = lv_obj_get_style_radius(obj, LV_PART_MAIN);

    lv_anim_t animation_fly_out;
    lv_anim_init(&animation_fly_out);
    lv_anim_set_exec_cb(&animation_fly_out, (lv_anim_exec_xcb_t)anim_x_cb);
    lv_anim_set_var(&animation_fly_out, obj);
    lv_anim_set_duration(&animation_fly_out, ANIMATION_TIME);
    lv_anim_set_values(&animation_fly_out, corner_radius, lv_obj_get_width(obj)); // Move out to the right
    lv_anim_t *running_anim = lv_anim_start(&animation_fly_out);
    return running_anim;
}

bool quick_drawer_is_hidden(lv_obj_t *obj)
{
    if (!obj)
        return true;

    // Check if the x position is beyond the screen width
    if (lv_obj_get_x(obj) >= lv_obj_get_width(lv_screen_active()))
        return true;

    return false;
}

static void btn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *parent = lv_obj_get_parent(btn);
    quick_drawer_user_data_t *user_data = (quick_drawer_user_data_t *)lv_obj_get_user_data(parent);
    int32_t btn_id = (int32_t)lv_event_get_user_data(e);
    switch (code)
    {
    case LV_EVENT_CLICKED:
        user_data->btn_pressed = btn_id;
        lv_obj_send_event(parent, LV_EVENT_VALUE_CHANGED, (void *)(intptr_t)btn_id);
        break;
    default:
        break;
    }
}