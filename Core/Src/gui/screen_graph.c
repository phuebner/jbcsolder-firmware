#include "screen_graph.h"
#include "theme.h"

lv_obj_t *create_graph_screen(lv_obj_t *parent)
{
    lv_obj_t *scr_root = lv_obj_create(parent, NULL);
    lv_obj_set_size(scr_root, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(scr_root, 0, LV_VER_RES);
    // lv_scr_load(scr_root);

    // Return button
    // lv_obj_t *btn_return = lv_btn_create(parent, NULL);
    // lv_theme_apply(btn_return, (lv_theme_style_t)CUSTOM_THEME_SIDEBAR_BTN);

    // // const lv_coord_t _corner_radius = lv_obj_get_style_radius(btn_return, LV_OBJ_PART_MAIN); // Get corner radius of button from style

    // lv_obj_set_size(btn_return, 40, 40);
    // lv_obj_set_pos(btn_return, 0, 0);

    // lv_btn_set_checkable(btn_return, false);
    // lv_btn_set_layout(btn_return, LV_LAYOUT_OFF); // This allows us to align the label of the button manually

    // lv_obj_t *lbl_btn_return = lv_label_create(btn_return, NULL);
    // lv_label_set_text(lbl_btn_return, LV_SYMBOL_BARS);
    // lv_obj_align(lbl_btn_return, NULL, LV_ALIGN_CENTER, 0, 0);
    // // lv_obj_set_event_cb(btn_return, return_to_home_event_cb);
    // lv_obj_set_ext_click_area(btn_return, 0, 20, 0, 20);

    /*Create a chart*/
    lv_obj_t *chart;
    chart = lv_chart_create(scr_root, NULL);
    lv_obj_set_size(chart, 200, 150);
    lv_obj_align(chart, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE); /*Show lines and points too*/
    lv_obj_set_drag_parent(chart, true);

    /*Add two data series*/
    lv_chart_series_t *temperature_series = lv_chart_add_series(chart, LV_COLOR_RED);
    lv_chart_series_t *power_series = lv_chart_add_series(chart, LV_COLOR_GREEN);

    /*Set the next points on 'ser1'*/
    lv_chart_set_next(chart, temperature_series, 10);
    lv_chart_set_next(chart, temperature_series, 10);
    lv_chart_set_next(chart, temperature_series, 10);
    lv_chart_set_next(chart, temperature_series, 10);
    lv_chart_set_next(chart, temperature_series, 10);
    lv_chart_set_next(chart, temperature_series, 10);
    lv_chart_set_next(chart, temperature_series, 10);
    lv_chart_set_next(chart, temperature_series, 30);
    lv_chart_set_next(chart, temperature_series, 70);
    lv_chart_set_next(chart, temperature_series, 90);

    /*Directly set points on 'ser2'*/
    power_series->points[0] = 90;
    power_series->points[1] = 70;
    power_series->points[2] = 65;
    power_series->points[3] = 65;
    power_series->points[4] = 65;
    power_series->points[5] = 65;
    power_series->points[6] = 65;
    power_series->points[7] = 65;
    power_series->points[8] = 65;
    power_series->points[9] = 65;

    lv_chart_refresh(chart); /*Required after direct set*/

    return scr_root;
}