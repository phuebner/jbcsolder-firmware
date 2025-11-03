#include "page_temperature_graph.h"
#include <theme.h>

/*
 * Simple real-time temperature / power graph.
 * - Creates two subjects (temperature float & power int)
 * - Starts a timer that reads the iron and updates the subjects
 * - Appends points to an LVGL chart (temperature on primary Y, power on secondary Y)
 */

typedef struct
{
    iron_t *iron;
    // lv_subject_t *subj_temperature;
    // lv_subject_t *subj_power;
    lv_obj_t *chart;
    lv_chart_series_t *ser_temp;
    lv_chart_series_t *ser_power;
    lv_timer_t *timer;
} page_temp_graph_ud_t;

static void page_temp_graph_timer_cb(lv_timer_t *timer)
{
    page_temp_graph_ud_t *ud = (page_temp_graph_ud_t *)lv_timer_get_user_data(timer);
    if (!ud || !ud->iron)
        return;

    /* Read values from iron */
    float temp = (float)iron_get_temperature(ud->iron); /* int cast in API, but represents degrees */
    float power = iron_get_power(ud->iron);

    // /* Update subjects so other UI can bind */
    // if (ud->subj_temperature)
    //     lv_subject_set_float(ud->subj_temperature, temp);
    // if (ud->subj_power)
    //     lv_subject_set_int(ud->subj_power, (int32_t)power);

    /* Append to chart: chart expects integer coords (lv_coord_t) */
    if (ud->chart && ud->ser_temp)
    {
        lv_chart_set_next_value(ud->chart, ud->ser_temp, (lv_coord_t)temp);
    }
    if (ud->chart && ud->ser_power)
    {
        /* map power to chart secondary axis range - but keep integer */
        lv_chart_set_next_value(ud->chart, ud->ser_power, (lv_coord_t)power);
    }
}

lv_obj_t *page_temperature_graph_create(lv_obj_t *parent, iron_t *iron)
{
    /* Allocate and init userdata */
    page_temp_graph_ud_t *ud = lv_malloc(sizeof(page_temp_graph_ud_t));
    lv_memzero(ud, sizeof(page_temp_graph_ud_t));
    ud->iron = iron;

    /* Create the container */
    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_remove_style_all(cont);
    lv_obj_set_layout(cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));

    // /* Create subjects for external bindings */
    // ud->subj_temperature = lv_malloc(sizeof(lv_subject_t));
    // lv_subject_init_float(ud->subj_temperature, iron ? (float)iron_get_temperature(iron) : 0.0f);

    // ud->subj_power = lv_malloc(sizeof(lv_subject_t));
    // lv_subject_init_int(ud->subj_power, iron ? (int32_t)iron_get_power(iron) : 0);

    /* Create container for chart and scale */
    lv_obj_t *chart_cont = lv_obj_create(cont);
    lv_obj_remove_style_all(chart_cont);
    lv_obj_set_size(chart_cont, lv_pct(100), lv_pct(90));
    lv_obj_set_flex_flow(chart_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(chart_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_ver(chart_cont, 10, LV_PART_MAIN);
    lv_obj_set_style_border_width(chart_cont, 0, LV_PART_MAIN);

    // /* Left scale label for temperature */
    // lv_obj_t *left_scale = lv_label_create(chart_cont);
    // lv_label_set_text(left_scale, "°C");
    // lv_obj_set_style_text_align(left_scale, LV_TEXT_ALIGN_RIGHT, 0);

    // /* Right scale label for power */
    // lv_obj_t *right_scale = lv_label_create(chart_cont);
    // lv_label_set_text(right_scale, "PWR");
    // lv_obj_set_style_text_align(right_scale, LV_TEXT_ALIGN_LEFT, 0);

    /* Create chart */
    lv_obj_t *chart = lv_chart_create(chart_cont);
    ud->chart = chart;
    lv_obj_set_height(chart, lv_pct(100));
    lv_obj_set_flex_grow(chart, 1);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_obj_set_style_size(chart, 0, 0, LV_PART_INDICATOR);
    lv_chart_set_point_count(chart, 120); /* keep 120 points (~12s at 100ms) */
    lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);
    lv_chart_set_div_line_count(chart, 4, 5);
    lv_obj_set_style_border_width(chart, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(chart, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_top(chart, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(chart, 1, LV_PART_MAIN);
    lv_obj_set_style_pad_left(chart, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_right(chart, 1, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(chart, LV_OPA_TRANSP, LV_PART_MAIN);

    /* Primary Y range for temperature (degrees C) */
    lv_chart_set_axis_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 500);
    /* Secondary Y range for power (PID output) - use small range (20 = granularity) */
    lv_chart_set_axis_range(chart, LV_CHART_AXIS_SECONDARY_Y, 0, 22);

    /* Create two series */
    ud->ser_temp = lv_chart_add_series(chart, COLOR_PRIMARY, LV_CHART_AXIS_PRIMARY_Y);                      /* orange */
    ud->ser_power = lv_chart_add_series(chart, lv_color_make(0x00, 0x99, 0xff), LV_CHART_AXIS_SECONDARY_Y); /* blue */

    /* Initialize series points to 0 */
    // lv_chart_poi(chart, ud->ser_temp, 0);
    // lv_chart_init_points(chart, ud->ser_power, 0);

    // Create a scale for the temperature on the left*/
    lv_obj_t *scale_temperature = lv_scale_create(chart_cont);
    lv_obj_move_to_index(scale_temperature, 0);
    lv_scale_set_mode(scale_temperature, LV_SCALE_MODE_VERTICAL_LEFT);
    lv_scale_set_total_tick_count(scale_temperature, 4);
    lv_scale_set_major_tick_every(scale_temperature, 1);
    lv_obj_set_style_pad_ver(scale_temperature, lv_chart_get_first_point_center_offset(chart), LV_PART_MAIN);
    lv_obj_set_style_line_width(scale_temperature, 0, LV_PART_MAIN);
    lv_obj_set_style_line_width(scale_temperature, 0, LV_PART_INDICATOR);
    lv_scale_set_range(scale_temperature, 0, 500);
    lv_obj_set_size(scale_temperature, 40, lv_pct(100));

    // Create a scale for Power
    lv_obj_t *scale_pwr = lv_scale_create(chart_cont);
    lv_obj_move_to_index(scale_pwr, 2);
    lv_scale_set_mode(scale_pwr, LV_SCALE_MODE_VERTICAL_RIGHT);
    lv_scale_set_total_tick_count(scale_pwr, 4);
    lv_scale_set_major_tick_every(scale_pwr, 1);
    lv_obj_set_style_pad_ver(scale_pwr, lv_chart_get_first_point_center_offset(chart), LV_PART_MAIN);
    lv_obj_set_style_line_width(scale_pwr, 0, LV_PART_MAIN);
    lv_obj_set_style_line_width(scale_pwr, 0, LV_PART_INDICATOR);
    lv_scale_set_range(scale_pwr, 0, 22);
    lv_obj_set_size(scale_pwr, 30, lv_pct(100));

    // static const char * month[] = {"Jan", "Febr", "March", "Apr", "May", "Jun", "July", "Aug", "Sept", "Oct", "Nov", "Dec", NULL};

    /* Store user data on container so it can be cleaned up later if needed */
    lv_obj_set_user_data(cont, ud);

    /* Create timer to update subjects and chart every 100ms */
    ud->timer = lv_timer_create(page_temp_graph_timer_cb, 100, ud);

    return cont;
}