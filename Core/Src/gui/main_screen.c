/*
 * main_screen.c
 *
 *  Created on: Dec 20, 2020
 *      Author: patrick
 */

#include <stdint.h>
#include "lvgl/lvgl.h"
#include "../hal_lvgl_encoder.h"
#include "theme.h"
#include "../iron.h"
#include "main_screen.h"

/* -------------------------------------------------------------------------- */
/*                                   DEFINES                                  */
/* -------------------------------------------------------------------------- */

#define SETPOINT_STEP_SIZE	5
#define ANIMATION_TIME		200

/* --------------------------------- Colors --------------------------------- */
#define COLOR_BG_TITLEBAR			lv_color_hex(0xA00101) // dark red
#define COLOR_BG_PRESET_DRAWER		lv_color_hex(0x4D4D4D) // dark grey


/* -------------------------------------------------------------------------- */
/*                              STATIC PROTOTYPES                             */
/* -------------------------------------------------------------------------- */

static void setup_styles(void);
static void setup_encoder_target(void);
static void setup_titlebar(void);
static void setup_preset_drawer(void);
static void setup_main_screen(void);
static void setup_sleep_screen(void);
LV_EVENT_CB_DECLARE(encoder_target_event_cb);
LV_EVENT_CB_DECLARE(btn_inc_setpoint_event_cb);
LV_EVENT_CB_DECLARE(btn_dec_setpoint_event_cb);
static void btn_quick_event_cb(lv_obj_t * obj, lv_event_t event);
static void main_screen_btn_en_event_cb(lv_obj_t * obj, lv_event_t event);
static void main_screen_refresher_task(struct _lv_task_t *);


const char* state_str[] = {
	[IRON_STATE_NOT_CONNECTED] = "Not Connected",
	[IRON_STATE_OFF] = "OFF",
	[IRON_STATE_HIBERNATE] = "Hibernate",
	[IRON_STATE_SLEEP] = "Sleep",
	[IRON_STATE_ACTIVE] = "Active",
};

/* -------------------------------------------------------------------------- */
/*                              STATIC VARIABLES                              */
/* -------------------------------------------------------------------------- */

static lv_style_t sty_current_temp;
static lv_style_t sty_lbl_unit;

static lv_obj_t *scr_home;

static lv_obj_t *box_titlebar;
static lv_obj_t *box_presets;
static lv_obj_t *box_main;
static lv_obj_t *box_sleep;

static lv_obj_t *lbl_iron_state;
static lv_obj_t *box_temperature;
static lv_obj_t *lbl_temperature;
static lv_obj_t *lbl_setpoint;
static lv_obj_t *lbl_power_bar;
static lv_obj_t * bar_power;
static lv_obj_t * btn_quick1;
static lv_obj_t * btn_quick2;
static lv_obj_t * btn_quick3;

static lv_obj_t *lbl_iron_state_title;
static lv_obj_t *lbl_iron_state_msg;

static lv_group_t*  g; 

/* -------------------------------------------------------------------------- */
/*                              PUBLIC FUNCTIONS                              */
/* -------------------------------------------------------------------------- */

/**
 * @brief Initialize the user interface
 * 
 */
void gui_init(void)
{
	scr_home = lv_obj_create(NULL, NULL);
	lv_scr_load(scr_home);

	lv_theme_t *theme = theme_init();
	lv_theme_set_act(theme);
	setup_styles();
	setup_encoder_target();
	setup_titlebar();
	setup_preset_drawer();
	setup_main_screen();
	setup_sleep_screen();


	lv_task_create(main_screen_refresher_task, 10, LV_TASK_PRIO_MID, NULL);
}

/* -------------------------------------------------------------------------- */
/*                              STATIC FUNCTIONS                              */
/* -------------------------------------------------------------------------- */

static void setup_styles(void)
{
	lv_style_init(&sty_current_temp);
	lv_style_set_text_font(&sty_current_temp, LV_STATE_DEFAULT, &lv_font_roboto_80);

	lv_style_init(&sty_lbl_unit);
	lv_style_set_text_font(&sty_lbl_unit, LV_STATE_DEFAULT, &lv_font_roboto_40);

}

static void setup_encoder_target(void)
{
	/* Setup hidden object as target for encoder events */
	g = lv_group_create();
    lv_indev_set_group(enc_indev, g);
	lv_obj_t * encoder_target = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_hidden(encoder_target, true);
	lv_group_add_obj(g, encoder_target);
	lv_obj_set_event_cb(encoder_target, encoder_target_event_cb);
	lv_group_focus_obj(encoder_target);
	lv_group_set_editing(g, true);
}

static void setup_titlebar(void)
{
    box_titlebar = lv_obj_create(lv_scr_act(), NULL);
    lv_theme_apply(box_titlebar, (lv_theme_style_t) CUSTOM_THEME_TITLE_BG);
 	lv_obj_set_size(box_titlebar, LV_HOR_RES, TITLEBAR_HEIGHT);

	/* Menu button */
	lv_obj_t * btn_menu = lv_btn_create(box_titlebar, NULL);
	lv_obj_set_size(btn_menu, TITLEBAR_HEIGHT, TITLEBAR_HEIGHT);
	lv_btn_set_checkable(btn_menu, true);
	lv_theme_apply(btn_menu, (lv_theme_style_t) CUSTOM_THEME_TITLEBAR_BTN);
	// lv_obj_set_style_local_value_str(btn_menu, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_POWER);
	// lv_obj_set_event_cb(btn_menu, main_screen_btn_menu_event_cb);

	lv_obj_t * lbl_btn_menu = lv_label_create(btn_menu, NULL);
    lv_label_set_text(lbl_btn_menu, LV_SYMBOL_BARS);
	lv_obj_set_pos(btn_menu, 5, 0);

	/* Button to enable solder iron */
	lv_obj_t * btn_en = lv_btn_create(box_titlebar, NULL);
	lv_obj_set_size(btn_en, TITLEBAR_HEIGHT, TITLEBAR_HEIGHT);
	lv_btn_set_checkable(btn_en, true);
	lv_theme_apply(btn_en, (lv_theme_style_t) CUSTOM_THEME_TITLEBAR_BTN);
	// lv_obj_set_style_local_value_str(btn_en, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_POWER);
	lv_obj_set_event_cb(btn_en, main_screen_btn_en_event_cb);

	lv_obj_t * lbl_btn_en = lv_label_create(btn_en, NULL);
    lv_label_set_text(lbl_btn_en, LV_SYMBOL_POWER);
	lv_obj_align_x(btn_en, btn_menu, LV_ALIGN_OUT_RIGHT_MID, 5);
}

static void setup_preset_drawer(void)
{
	const lv_coord_t X_HIDDEN = 30; /** Hidden portion of preset drawer to hide round corners */

	/* Preset drawer background */
	box_presets = lv_obj_create(lv_scr_act(), NULL);
    lv_theme_apply(box_presets, (lv_theme_style_t) CUSTOM_THEME_PRESET_DRAWER);
 	lv_obj_set_size(box_presets, PRESET_DRAWER_WIDTH + X_HIDDEN, PRESET_DRAWER_HEIGHT);
	lv_obj_align(box_presets, NULL, LV_ALIGN_IN_RIGHT_MID, X_HIDDEN, 0);

	/* Object mask to mask round corner of buttons */
    lv_obj_t * om = lv_objmask_create(box_presets, NULL);
    lv_obj_set_size(om, PRESET_DRAWER_WIDTH+ X_HIDDEN, PRESET_DRAWER_HEIGHT);
    lv_obj_align(om, NULL, LV_ALIGN_IN_LEFT_MID, 0, 0);

	lv_area_t mask_rect;
	lv_coord_t mask_radius;
	lv_draw_mask_radius_param_t mask_param;

	mask_rect.x1 = 0;
	mask_rect.y1 = 0;
	mask_rect.x2 = PRESET_DRAWER_WIDTH + X_HIDDEN;
	mask_rect.y2 = PRESET_DRAWER_HEIGHT;
	mask_radius = (lv_coord_t) lv_obj_get_style_radius(box_presets, LV_OBJ_PART_MAIN);
	lv_draw_mask_radius_init(&mask_param, &mask_rect, mask_radius, false);
	lv_objmask_add_mask(om, &mask_param);

	/* Preset button 1*/
	const lv_coord_t PRESET_BUTTON_HEIGHT = PRESET_DRAWER_HEIGHT / 3;

	btn_quick1 = lv_btn_create(om, NULL);
	 lv_theme_apply(btn_quick1, (lv_theme_style_t) CUSTOM_THEME_PRESET_BTN);
	lv_obj_set_size(btn_quick1, PRESET_DRAWER_WIDTH , PRESET_BUTTON_HEIGHT);
	lv_obj_align(btn_quick1, NULL, LV_ALIGN_CENTER, -(X_HIDDEN/2), -(PRESET_BUTTON_HEIGHT));

	lv_obj_t * lbl_btn_quick1 = lv_label_create(btn_quick1, NULL);
	lv_label_set_text(lbl_btn_quick1, "350");
	lv_obj_set_event_cb(btn_quick1, btn_quick_event_cb);

	/* Preset button 2*/
	btn_quick2 = lv_btn_create(om, NULL);
	lv_theme_apply(btn_quick2, (lv_theme_style_t) CUSTOM_THEME_PRESET_BTN);
	lv_obj_set_size(btn_quick2, PRESET_DRAWER_WIDTH , PRESET_BUTTON_HEIGHT);
	lv_obj_align(btn_quick2, NULL, LV_ALIGN_CENTER, -(X_HIDDEN/2), 0);

	lv_obj_t * lbl_btn_quick2 = lv_label_create(btn_quick2, NULL);
	lv_label_set_text(lbl_btn_quick2, "300");
	lv_obj_set_event_cb(btn_quick2, btn_quick_event_cb);

	/* Preset button 3 */
	btn_quick3 = lv_btn_create(om, NULL);
	lv_theme_apply(btn_quick3, (lv_theme_style_t) CUSTOM_THEME_PRESET_BTN);
	lv_obj_set_size(btn_quick3, PRESET_DRAWER_WIDTH , PRESET_BUTTON_HEIGHT);
	lv_obj_align(btn_quick3, NULL, LV_ALIGN_CENTER, -(X_HIDDEN/2), PRESET_BUTTON_HEIGHT);

	lv_obj_t * lbl_btn_quick3 = lv_label_create(btn_quick3, NULL);
	lv_label_set_text(lbl_btn_quick3, "250");
	lv_obj_set_event_cb(btn_quick3, btn_quick_event_cb);

	/* Seperators */
	static lv_point_t line_points[] = { {0,0}, {PRESET_DRAWER_WIDTH-10, 0} };
	
	lv_obj_t * line1 = lv_line_create(om, NULL);
	lv_line_set_points(line1, line_points, 2);
	lv_theme_apply(line1, (lv_theme_style_t) CUSTOM_THEME_PRESET_SEPARATOR);
	lv_obj_align(line1, NULL, LV_ALIGN_CENTER, -(X_HIDDEN/2), -(PRESET_BUTTON_HEIGHT/2));

	lv_obj_t * line2 = lv_line_create(om, NULL);
	lv_line_set_points(line2, line_points, 2);
	lv_theme_apply(line2, (lv_theme_style_t) CUSTOM_THEME_PRESET_SEPARATOR);
	lv_obj_align(line2, NULL, LV_ALIGN_CENTER, -(X_HIDDEN/2), (PRESET_BUTTON_HEIGHT/2));

	/* Animate in */
	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_var(&a, box_presets);
	lv_anim_set_time(&a, ANIMATION_TIME);
	lv_anim_set_delay(&a, 300);
	lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) lv_obj_set_x);
	lv_anim_set_values(&a, lv_obj_get_x(box_presets) + PRESET_DRAWER_WIDTH,
			lv_obj_get_x(box_presets));
	lv_anim_start(&a);

	lv_obj_fade_in(box_presets, ANIMATION_TIME - 50, 300);
}

static void setup_main_screen(void)
{
	/* box main */
	lv_coord_t box_main_width = LV_HOR_RES_MAX - PRESET_DRAWER_WIDTH;
	box_main = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_clean_style_list(box_main, LV_OBJ_PART_MAIN);
 	lv_obj_set_size(box_main, LV_HOR_RES_MAX - PRESET_DRAWER_WIDTH, LV_VER_RES_MAX - TITLEBAR_HEIGHT);
	lv_obj_set_pos(box_main, 0, TITLEBAR_HEIGHT);

	/* Label Iron Status */
	lbl_iron_state = lv_label_create(box_main, NULL);
	lv_label_set_long_mode(lbl_iron_state, LV_LABEL_LONG_CROP);
	lv_label_set_align(lbl_iron_state, LV_LABEL_ALIGN_CENTER);
	lv_obj_set_size(lbl_iron_state, box_main_width, lv_font_montserrat_24.line_height);
	lv_label_set_text(lbl_iron_state, "OFF");
	lv_obj_align(lbl_iron_state, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);
	lv_obj_set_style_local_text_font(lbl_iron_state, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_24);

	/* Temperature container */
	box_temperature = lv_obj_create(box_main, NULL);
	lv_obj_set_style_local_bg_opa(box_temperature, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_opa(box_temperature, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);

	/* Current temperature display */
	lbl_temperature = lv_label_create(box_temperature, NULL);
	lv_obj_add_style(lbl_temperature, LV_LABEL_PART_MAIN, &sty_current_temp);
	lv_label_set_long_mode(lbl_temperature, LV_LABEL_LONG_CROP);
	lv_obj_set_size(lbl_temperature, 140, 70);
	lv_label_set_align(lbl_temperature, LV_LABEL_ALIGN_RIGHT);
	lv_label_set_text(lbl_temperature, "200");

	/* Current Temperature Unit */
	lv_obj_t *lbl_unit = lv_label_create(box_temperature, NULL);
	lv_obj_add_style(lbl_unit, LV_LABEL_PART_MAIN, &sty_lbl_unit);
	lv_label_set_align(lbl_unit, LV_LABEL_ALIGN_RIGHT);
	lv_label_set_text(lbl_unit, "°C");

	/* Align measurement and unit within container */
	lv_coord_t w = lv_obj_get_width(lbl_temperature) + lv_obj_get_width(lbl_unit) + (2*4);
	lv_obj_set_size(box_temperature, w, 70);
	lv_obj_align(lbl_temperature, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 4, 5);
	lv_obj_align(lbl_unit, lbl_temperature, LV_ALIGN_OUT_RIGHT_TOP, 0, -3);

	/* Align temperature container*/
	lv_obj_align(box_temperature, NULL, LV_ALIGN_CENTER, 0, -25);

	/* Temperature setpoint label */
	lbl_setpoint = lv_label_create(box_main, NULL);
	lv_label_set_long_mode(lbl_setpoint, LV_LABEL_LONG_CROP);
	lv_obj_set_size(lbl_setpoint, w - 90, lv_font_roboto_28.line_height);
	lv_obj_set_style_local_text_font(lbl_setpoint, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_roboto_28);
	lv_label_set_align(lbl_setpoint, LV_LABEL_ALIGN_CENTER);
	lv_label_set_text(lbl_setpoint, "200°C");
	lv_obj_align(lbl_setpoint, NULL, LV_ALIGN_CENTER, 0, 35);

	/* Button to increase setpoint */
	lv_obj_t * btn_inc_setpoint = lv_btn_create(box_main, NULL);
	lv_obj_set_size(btn_inc_setpoint, 47, 34);
	lv_obj_align(btn_inc_setpoint, lbl_setpoint, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
	lv_theme_apply(btn_inc_setpoint, LV_THEME_SPINBOX_BTN);
	// lv_obj_set_style_local_text_font(btn_inc_setpoint, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_24);
	lv_obj_set_style_local_value_str(btn_inc_setpoint, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_PLUS);
	lv_obj_set_event_cb(btn_inc_setpoint, btn_inc_setpoint_event_cb);

	/* Button to decrease setpoint */
	lv_obj_t * btn_dec_setpoint = lv_btn_create(box_main, NULL);
	lv_obj_set_size(btn_dec_setpoint, 47, 34);
	lv_obj_align(btn_dec_setpoint, lbl_setpoint, LV_ALIGN_OUT_LEFT_MID, 0, 0);
	lv_theme_apply(btn_dec_setpoint, LV_THEME_SPINBOX_BTN);
	// lv_obj_set_style_local_text_font(btn_dec_setpoint, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_24);
	lv_obj_set_style_local_value_str(btn_dec_setpoint, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_MINUS);
	lv_obj_set_event_cb(btn_dec_setpoint, btn_dec_setpoint_event_cb);

	/* Box Power Bar */
	lv_obj_t * box_power_bar = lv_obj_create(box_main, NULL);
    lv_obj_clean_style_list(box_power_bar, LV_OBJ_PART_MAIN);
 	lv_obj_set_size(box_power_bar, 60, 45);
	lv_obj_align(box_power_bar, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);

	/* Label Power Bar */
	lbl_power_bar = lv_label_create(box_main, NULL);
	lv_label_set_align(lbl_power_bar, LV_LABEL_ALIGN_CENTER);
	lv_label_set_text(lbl_power_bar, "Power\n4\%");
	lv_obj_set_style_local_text_font(lbl_power_bar, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_roboto_14);
	lv_obj_align(lbl_power_bar, box_power_bar, LV_ALIGN_CENTER, 0, 0);

	/* Power Bar */
    bar_power = lv_bar_create(box_main, NULL);
	lv_theme_apply(bar_power, (lv_theme_style_t) CUSTOM_THEME_POWER_BAR);
    lv_obj_set_size(bar_power, LV_HOR_RES_MAX - PRESET_DRAWER_WIDTH - 70, 25);
    lv_bar_set_anim_time(bar_power, 500);
    lv_bar_set_range(bar_power, 0, 20);
    lv_bar_set_value(bar_power, 0, LV_ANIM_ON);
    lv_obj_align(bar_power, box_power_bar, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
}

static void setup_sleep_screen(void)
{
	/* box sleep */
	box_sleep = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_clean_style_list(box_sleep, LV_OBJ_PART_MAIN);
 	lv_obj_set_size(box_sleep, LV_HOR_RES_MAX, LV_VER_RES_MAX - TITLEBAR_HEIGHT);
	lv_obj_set_pos(box_sleep, 0, TITLEBAR_HEIGHT);

	/* Label Iron State Title */
	lbl_iron_state_title = lv_label_create(box_sleep, NULL);
	lv_label_set_long_mode(lbl_iron_state_title, LV_LABEL_LONG_CROP);
	lv_label_set_align(lbl_iron_state_title, LV_LABEL_ALIGN_LEFT);
	lv_obj_set_size(lbl_iron_state_title, LV_HOR_RES_MAX, 50);
	lv_label_set_text(lbl_iron_state_title, "");
	lv_obj_align(lbl_iron_state_title, NULL, LV_ALIGN_IN_LEFT_MID, 10, -20);
	lv_obj_set_style_local_text_font(lbl_iron_state_title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_roboto_40);

	/* Label Iron State_MSG */
	lbl_iron_state_msg = lv_label_create(box_sleep, NULL);
	lv_label_set_long_mode(lbl_iron_state_msg, LV_LABEL_LONG_BREAK);
	lv_label_set_align(lbl_iron_state_msg, LV_LABEL_ALIGN_LEFT);
	lv_obj_set_size(lbl_iron_state_msg, LV_HOR_RES_MAX, 100);
	lv_label_set_text(lbl_iron_state_msg, "");
	lv_obj_align(lbl_iron_state_msg, NULL, LV_ALIGN_IN_LEFT_MID, 10, 20);
	lv_obj_set_style_local_text_font(lbl_iron_state_msg, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_roboto_20);
}

/* -------------------------------------------------------------------------- */
/*                               EVENT CALLBACKS                              */
/* -------------------------------------------------------------------------- */

LV_EVENT_CB_DECLARE(encoder_target_event_cb) 
{
	if(e == LV_EVENT_KEY) {
		const uint32_t * key = lv_event_get_data();

		switch(*key) {
		case LV_KEY_RIGHT:
			iron_set_setpoint(iron_get_setpoint() + SETPOINT_STEP_SIZE);
			break;
		case LV_KEY_LEFT:
			iron_set_setpoint(iron_get_setpoint() - SETPOINT_STEP_SIZE);
			break;
		}
	}
}

LV_EVENT_CB_DECLARE(btn_inc_setpoint_event_cb)
{
	if(e == LV_EVENT_CLICKED)
		iron_set_setpoint(iron_get_setpoint() + SETPOINT_STEP_SIZE);
}

LV_EVENT_CB_DECLARE(btn_dec_setpoint_event_cb)
{
	if(e == LV_EVENT_CLICKED)
		iron_set_setpoint(iron_get_setpoint() - SETPOINT_STEP_SIZE);
}


static void btn_quick_event_cb(lv_obj_t * obj, lv_event_t event)
{
	if(obj == btn_quick1)
	{
		iron_set_setpoint(350);
	}
	else if(obj == btn_quick2)
	{
		iron_set_setpoint(300);
	}
	else if(obj == btn_quick3)
	{
		iron_set_setpoint(250);
	}
}
// static void main_screen_iron_state_change_cb(iron_state_t state)
// {
// 	lv_label_set_text(lbl_iron_state, state_str[state]);
// }

static void main_screen_btn_en_event_cb(lv_obj_t * obj, lv_event_t event)
{
    switch(event) {
        case LV_EVENT_VALUE_CHANGED:
        	iron_set_enable(lv_btn_get_state(obj) == LV_BTN_STATE_CHECKED_RELEASED);
            break;
    }
}

static void main_screen_refresher_task(struct _lv_task_t *taskh)
{
    static uint16_t prev_temperature;
    static uint16_t prev_setpoint;
	static iron_state_t prev_iron_state;
	static int16_t prev_pwr;

//    static uint16_t prev_value_set = 0;

//    uint16_t temp_is = iron_temperature();
//    if(prev_value_is != temp_is)
//    {
//        if(lv_obj_get_screen(label_temp) == lv_scr_act())
//        {
	iron_state_t new_iron_state = iron_get_state();
	if(new_iron_state != prev_iron_state)
	{
		if(new_iron_state == IRON_STATE_ACTIVE)
		{
			/* Animate out sleep */
			lv_anim_t anim_out_sleep;
			lv_anim_init(&anim_out_sleep);
			lv_anim_set_var(&anim_out_sleep, box_sleep);
			lv_anim_set_time(&anim_out_sleep, ANIMATION_TIME);
			lv_anim_set_delay(&anim_out_sleep, 0);
			lv_anim_set_exec_cb(&anim_out_sleep, (lv_anim_exec_xcb_t) lv_obj_set_x);
			lv_anim_set_values(&anim_out_sleep, 0, -LV_HOR_RES_MAX);

			/* Animate in preset */
			lv_anim_t anim_in_preset;
			lv_anim_init(&anim_in_preset);
			lv_anim_set_var(&anim_in_preset, box_presets);
			lv_anim_set_time(&anim_in_preset, ANIMATION_TIME);
			lv_anim_set_delay(&anim_in_preset, ANIMATION_TIME);
			lv_anim_set_exec_cb(&anim_in_preset, (lv_anim_exec_xcb_t) lv_obj_set_x);
			lv_anim_set_values(&anim_in_preset, LV_HOR_RES_MAX, LV_HOR_RES_MAX - PRESET_DRAWER_WIDTH);

			/* Animate in main */
			lv_anim_t anim_in_active;
			lv_anim_init(&anim_in_active);
			lv_anim_set_var(&anim_in_active, box_main);
			lv_anim_set_time(&anim_in_active, ANIMATION_TIME);
			lv_anim_set_delay(&anim_in_active, ANIMATION_TIME);
			lv_anim_set_exec_cb(&anim_in_active, (lv_anim_exec_xcb_t) lv_obj_set_x);
			lv_anim_set_values(&anim_in_active, -(LV_HOR_RES_MAX - PRESET_DRAWER_WIDTH), 0);
			
			/* Start animation */
			lv_anim_start(&anim_out_sleep);
			lv_anim_start(&anim_in_preset);
			lv_anim_start(&anim_in_active);
				
		}

		if(prev_iron_state == IRON_STATE_ACTIVE)
		{
			/* Animate out preset */
			lv_anim_t anim_out_preset;
			lv_anim_init(&anim_out_preset);
			lv_anim_set_var(&anim_out_preset, box_presets);
			lv_anim_set_time(&anim_out_preset, ANIMATION_TIME);
			lv_anim_set_delay(&anim_out_preset, 0);
			lv_anim_set_exec_cb(&anim_out_preset, (lv_anim_exec_xcb_t) lv_obj_set_x);
			lv_anim_set_values(&anim_out_preset, LV_HOR_RES_MAX - PRESET_DRAWER_WIDTH, LV_HOR_RES_MAX);

			/* Animate in main */
			lv_anim_t anim_out_active;
			lv_anim_init(&anim_out_active);
			lv_anim_set_var(&anim_out_active, box_main);
			lv_anim_set_time(&anim_out_active, ANIMATION_TIME);
			lv_anim_set_delay(&anim_out_active, 0);
			lv_anim_set_exec_cb(&anim_out_active, (lv_anim_exec_xcb_t) lv_obj_set_x);
			lv_anim_set_values(&anim_out_active, 0, -(LV_HOR_RES_MAX - PRESET_DRAWER_WIDTH));
			
			/* Animate in sleep */
			lv_anim_t anim_in_sleep;
			lv_anim_init(&anim_in_sleep);
			lv_anim_set_var(&anim_in_sleep, box_sleep);
			lv_anim_set_time(&anim_in_sleep, ANIMATION_TIME);
			lv_anim_set_delay(&anim_in_sleep, ANIMATION_TIME);
			lv_anim_set_exec_cb(&anim_in_sleep, (lv_anim_exec_xcb_t) lv_obj_set_x);
			lv_anim_set_values(&anim_in_sleep, -LV_HOR_RES_MAX, 0);

			/* Start animation */
			lv_anim_start(&anim_out_preset);
			lv_anim_start(&anim_out_active);
			lv_anim_start(&anim_in_sleep);
		}

		switch(new_iron_state) {
		case IRON_STATE_NOT_CONNECTED:
			lv_label_set_text(lbl_iron_state_title, "No Iron connected");
			lv_label_set_text(lbl_iron_state_msg, "");
			break;
		case IRON_STATE_OFF:
			lv_label_set_text(lbl_iron_state_title, "Iron disabled");
			lv_label_set_text_fmt(lbl_iron_state_msg, "Current temperature: %d°C\nEnable iron in menu", iron_get_temperature());
			break;
		case IRON_STATE_SLEEP:
			lv_label_set_text(lbl_iron_state_title, "Sleep");
			lv_label_set_text_fmt(lbl_iron_state_msg, "Current temperature: %d°C\nEntering hibernate in: %d seconds", iron_get_temperature(), iron_get_seconds_till_hibernate());
			break;
		case IRON_STATE_HIBERNATE:
			lv_label_set_text(lbl_iron_state_title, "Hibernate");
			lv_label_set_text_fmt(lbl_iron_state_msg, "Current temperature: %d°C", iron_get_temperature());
			break;
		case IRON_STATE_ACTIVE:
			lv_label_set_text(lbl_iron_state_title, "Active");
			lv_label_set_text_fmt(lbl_iron_state_msg, "Current temperature: %d°C", iron_get_temperature());
			break;
		}
		// lv_label_set_text(lbl_iron_state, state_str[new_iron_state]);
		prev_iron_state = new_iron_state;
	}



    uint16_t new_temperature = iron_get_temperature();
	if(new_temperature != prev_temperature)
	{
		switch(new_iron_state) {
		case IRON_STATE_OFF:
			lv_label_set_text_fmt(lbl_iron_state_msg, "Current temperature: %d°C\nEnable iron in menu", new_temperature);
			break;
		case IRON_STATE_SLEEP:
			lv_label_set_text_fmt(lbl_iron_state_msg, "Current temperature: %d°C\nEntering hibernate in: %d seconds", new_temperature, iron_get_seconds_till_hibernate());
			break;
		case IRON_STATE_HIBERNATE:
			lv_label_set_text_fmt(lbl_iron_state_msg, "Current temperature: %d°C", new_temperature);
			break;
		case IRON_STATE_ACTIVE:
			lv_label_set_text_fmt(lbl_temperature, "%d", new_temperature);
			break;
		}
		prev_temperature = new_temperature;
	}

	static uint32_t prev_time_till_hibernate;
	uint32_t new_time_till_hibernate = iron_get_seconds_till_hibernate();
	if(new_time_till_hibernate != prev_time_till_hibernate)
	{
		lv_label_set_text_fmt(lbl_iron_state_msg, "Current temperature: %d°C\nEntering hibernate in: %d seconds", new_temperature, new_time_till_hibernate);
		prev_time_till_hibernate = new_time_till_hibernate;
	}

	uint16_t new_setpoint = iron_get_setpoint();
	if(new_setpoint != prev_setpoint)
	{
		lv_label_set_text_fmt(lbl_setpoint, "%d°C", new_setpoint);
		prev_setpoint = new_setpoint;
	}

	int16_t new_pwr = (int16_t) iron_get_power();
	if(new_pwr != prev_pwr)
	{
		lv_bar_set_value(bar_power, new_pwr, LV_ANIM_OFF);
		prev_pwr = new_pwr;
	}
	



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