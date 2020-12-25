/*
 * main_screen.c
 *
 *  Created on: Dec 20, 2020
 *      Author: patrick
 */

#include "lvgl/lvgl.h"
#include "../iron.h"

LV_FONT_DECLARE(lv_font_montserrat_80);

static void main_screen_btn_en_event_cb(lv_obj_t * obj, lv_event_t event);
static void main_screen_refresher_task(struct _lv_task_t *);

static lv_style_t sty_current_temp;
static lv_style_t sty_lbl_unit;

static lv_obj_t *lbl_current_temp = NULL;

static void create_styles(void)
{
  lv_style_init(&sty_current_temp);
  lv_style_set_text_font(&sty_current_temp, LV_STATE_DEFAULT, &lv_font_montserrat_80);

  lv_style_init(&sty_lbl_unit);
  lv_style_set_text_font(&sty_lbl_unit, LV_STATE_DEFAULT, &lv_font_montserrat_42);
}

static void create_widgets(void)
{
  // lv_obj_t *btn = lv_btn_create(lv_scr_act(), NULL);
  // lv_obj_set_pos(btn, 10, 10);
  // lv_obj_t *btn_lbl = lv_label_create(btn, NULL);
  // lv_label_set_text(btn_lbl, "Start");

//  uint16_t temperature = 300;

	/* Container for current temperature and unit */
//	lv_obj_t *temp_container = lv_obj_create(lv_scr_act(), NULL);
//	lv_obj_set_size(temp_container, 200, 100);
//	lv_obj_align(temp_container, NULL, LV_ALIGN_CENTER, 0, 0);

	/* Current temperature display */
	lbl_current_temp = lv_label_create(lv_scr_act(), NULL);
	lv_obj_add_style(lbl_current_temp, LV_LABEL_PART_MAIN, &sty_current_temp);
	lv_label_set_long_mode(lbl_current_temp, LV_LABEL_LONG_CROP);
	lv_obj_set_size(lbl_current_temp, 200, 80);
	lv_label_set_align(lbl_current_temp, LV_LABEL_ALIGN_RIGHT);
	lv_label_set_text(lbl_current_temp, "---");
	lv_obj_align_origo(lbl_current_temp, NULL, LV_ALIGN_CENTER, -60, 0);

	/* Current Temperature Unit */
	lv_obj_t *lbl_unit = lv_label_create(lv_scr_act(), NULL);
	lv_obj_add_style(lbl_unit, LV_LABEL_PART_MAIN, &sty_lbl_unit);
	lv_label_set_align(lbl_unit, LV_LABEL_ALIGN_RIGHT);
	lv_label_set_text(lbl_unit, "°C");
	lv_obj_align(lbl_unit, lbl_current_temp, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);

	/* Button to enable solder iron */
	lv_obj_t * btn_en = lv_btn_create(lv_scr_act(), NULL);
	lv_btn_set_checkable(btn_en, true);
	lv_obj_set_event_cb(btn_en, main_screen_btn_en_event_cb);
	lv_obj_align(btn_en, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);


}

void gui_main_screen(void)
{
	create_styles();
	create_widgets();
//	lv_style_init(&style_box);
//    lv_style_set_value_align(&style_box, LV_STATE_DEFAULT, LV_ALIGN_OUT_TOP_LEFT);
//    lv_style_set_value_ofs_y(&style_box, LV_STATE_DEFAULT, -LV_DPX(10));
//    lv_style_set_margin_top(&style_box, LV_STATE_DEFAULT, LV_DPX(30));

//    lv_obj_t *parent = lv_scr_act();
//    lv_obj_t *h = lv_cont_create(parent, NULL);
//    lv_cont_set_layout(h, LV_LAYOUT_PRETTY_MID);
	lv_task_create(main_screen_refresher_task, 10, LV_TASK_PRIO_MID, NULL);
}

static void main_screen_btn_en_event_cb(lv_obj_t * obj, lv_event_t event)
{
    switch(event) {
        case LV_EVENT_PRESSED:
        	iron_setEnable(lv_btn_get_state(obj) == LV_BTN_STATE_CHECKED_PRESSED);
            break;
    }
}

static void main_screen_refresher_task(struct _lv_task_t *taskh)
{
//    static uint16_t prev_value_is = 0;
//    static uint16_t prev_value_set = 0;

//    uint16_t temp_is = iron_temperature();
//    if(prev_value_is != temp_is)
//    {
//        if(lv_obj_get_screen(label_temp) == lv_scr_act())
//        {
//    if(iron_isEnabled())
//    {
   	    lv_label_set_text_fmt(lbl_current_temp, "%d", iron_temperature());
//    }
//    else
//    {
//    	lv_label_set_text(lbl_current_temp, "OFF");
//    }
//   	        }
//        prev_value_is = temp_is;
//    }
//
//    uint16_t temp_setpoint = IRON_TempSetpoint();
//    if(prev_value_set != temp_setpoint)
//    {
//   	    if(lv_obj_get_screen(label_set_temp) == lv_scr_act())
//        {
//		    char buf[5];
//		    sprintf(buf, "%3d", temp_setpoint);
//		    lv_label_set_text(label_set_temp, buf);
//		}
//		prev_value_set = temp_setpoint;
//    }
}
