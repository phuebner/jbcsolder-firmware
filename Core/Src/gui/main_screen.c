/*
 * main_screen.c
 *
 *  Created on: Dec 20, 2020
 *      Author: patrick
 */

#include <stdint.h>
#include "lvgl.h"
#include "../hal_lvgl_encoder.h"
#include "theme.h"
#include "../iron.h"
#include "main_screen.h"
#include "screen_menu.h"
#include "buzzer.h"

/* -------------------------------------------------------------------------- */
/*                                   DEFINES                                  */
/* -------------------------------------------------------------------------- */

#define SETPOINT_STEP_SIZE 5

/* -------------------------------------------------------------------------- */
/*                              STATIC PROTOTYPES                             */
/* -------------------------------------------------------------------------- */
static void create_main_screen(void);

static void setup_styles(void);
static void setup_encoder_target(void);
static void setup_titlebar(void);
static void setup_preset_drawer(void);
static void setup_main_screen(void);
LV_EVENT_CB_DECLARE(encoder_target_event_cb);
LV_EVENT_CB_DECLARE(btn_inc_setpoint_event_cb);
LV_EVENT_CB_DECLARE(btn_dec_setpoint_event_cb);
static void btn_quick_event_cb(lv_obj_t *obj, lv_event_t event);
static void main_screen_iron_enable_event_cb(lv_obj_t *obj, lv_event_t event);
static void main_screen_refresher_task(struct _lv_task_t *);

static void switch_to_menu_event_cb(lv_obj_t *obj, lv_event_t event);
static void return_to_home_event_cb(lv_obj_t *obj, lv_event_t event);

const char *state_str[] = {
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
static lv_obj_t *scr_menu;

static lv_obj_t *box_titlebar;
static lv_obj_t *box_presets;
static lv_obj_t *box_temperature;
static lv_obj_t *box_setpoint;
static lv_obj_t *box_sleep;

static lv_obj_t *lbl_iron_id;
static lv_obj_t *lbl_temperature;
static lv_obj_t *lbl_setpoint;
static lv_obj_t *bar_power;
static lv_obj_t *btn_quick1;
static lv_obj_t *btn_quick2;
static lv_obj_t *btn_quick3;

static lv_obj_t *lbl_iron_state_title;
static lv_obj_t *lbl_iron_state_msg1;
static lv_obj_t *lbl_iron_state_msg2;

static lv_group_t *g;

static lv_task_t *main_screen_task_handle;
static bool initial_run;

/* Sizes */
static const lv_coord_t TITLEBAR_HEIGHT = 35;
static const lv_coord_t TITLEBAR_BUTTON_HEIGHT = 55;

static const lv_coord_t DRAWER_WIDTH = 65;
static const lv_coord_t DRAWER_PADDING_VER = 14;

static const lv_coord_t POWER_BAR_WIDTH = 15;
static const lv_coord_t POWER_BAR_PADDING_HOR = 10;
static const lv_coord_t POWER_BAR_PADDING_VER = 14;

static const lv_coord_t CENTER_AREA_WIDTH = (LV_HOR_RES_MAX - POWER_BAR_PADDING_HOR - POWER_BAR_WIDTH - DRAWER_WIDTH);
static const lv_point_t CENTER_AREA_POSITION = {x : POWER_BAR_WIDTH + POWER_BAR_PADDING_HOR, y : TITLEBAR_HEIGHT};

/* -------------------------------------------------------------------------- */
/*                              PUBLIC FUNCTIONS                              */
/* -------------------------------------------------------------------------- */

/**
 * @brief Initialize the user interface
 *
 */
void gui_init(void)
{

	lv_theme_t *theme = theme_init();
	lv_theme_set_act(theme);
	setup_styles();

	create_main_screen();
}

/* -------------------------------------------------------------------------- */
/*                              STATIC FUNCTIONS                              */
/* -------------------------------------------------------------------------- */

static void create_main_screen(void)
{
	scr_home = lv_obj_create(NULL, NULL);
	lv_scr_load(scr_home);
	setup_encoder_target();
	setup_main_screen();
	setup_titlebar();
	setup_preset_drawer();

	initial_run = true;
	main_screen_task_handle = lv_task_create(main_screen_refresher_task, 10, LV_TASK_PRIO_MID, &initial_run);
}

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
	lv_obj_t *encoder_target = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_hidden(encoder_target, true);
	lv_group_add_obj(g, encoder_target);
	lv_obj_set_event_cb(encoder_target, encoder_target_event_cb);
	lv_group_focus_obj(encoder_target);
	lv_group_set_editing(g, true);
}

static void setup_titlebar(void)
{
	box_titlebar = lv_obj_create(lv_scr_act(), NULL);
	lv_theme_apply(box_titlebar, (lv_theme_style_t)CUSTOM_THEME_TITLE_BG);
	lv_obj_set_size(box_titlebar, LV_HOR_RES, TITLEBAR_HEIGHT);

	/* Menu button */
	lv_obj_t *btn_menu = lv_btn_create(lv_scr_act(), NULL);
	lv_theme_apply(btn_menu, (lv_theme_style_t)CUSTOM_THEME_TITLEBAR_BTN);

	const lv_coord_t _corner_radius = lv_obj_get_style_radius(btn_menu, LV_OBJ_PART_MAIN); // Get corner radius of button from style
	const lv_coord_t _button_width = CENTER_AREA_POSITION.x + 40;						   // Button should start from the left edge and reach into the center area

	lv_obj_set_size(btn_menu, _button_width + _corner_radius, TITLEBAR_BUTTON_HEIGHT);
	lv_obj_set_pos(btn_menu, -_corner_radius, 0);

	lv_btn_set_checkable(btn_menu, false);
	lv_btn_set_layout(btn_menu, LV_LAYOUT_OFF); // This allows us to align the label of the button manually

	lv_obj_t *lbl_btn_menu = lv_label_create(btn_menu, NULL);
	lv_label_set_text(lbl_btn_menu, LV_SYMBOL_BARS);
	lv_obj_align(lbl_btn_menu, NULL, LV_ALIGN_CENTER, (_corner_radius / 2), 0);
	lv_obj_set_event_cb(btn_menu, switch_to_menu_event_cb);
	lv_obj_set_ext_click_area(btn_menu, 0, 20, 0, 20);
}

static void setup_preset_drawer(void)
{
	const lv_coord_t DRAWER_HEIGHT = (LV_VER_RES_MAX - (2 * DRAWER_PADDING_VER));

	/* Preset drawer create and apply style */
	box_presets = lv_obj_create(lv_scr_act(), NULL);
	lv_theme_apply(box_presets, (lv_theme_style_t)CUSTOM_THEME_PRESET_DRAWER);

	/* Set size and position */
	const lv_coord_t CORNER_RADIUS = lv_obj_get_style_radius(box_presets, LV_OBJ_PART_MAIN);
	lv_obj_set_size(box_presets, DRAWER_WIDTH + CORNER_RADIUS, DRAWER_HEIGHT); // Add corner radius to width to hide right corner
	lv_obj_align(box_presets, NULL, LV_ALIGN_IN_RIGHT_MID, CORNER_RADIUS, 0);

	/* Create round corners using object mask - this is needed because otherwise the pressed buttons would bleed over the round corners */
	lv_obj_t *om = lv_objmask_create(box_presets, NULL);
	lv_obj_set_size(om, DRAWER_WIDTH + CORNER_RADIUS, DRAWER_HEIGHT); // Add corner radius to width to hide right corner
	lv_obj_align(om, NULL, LV_ALIGN_IN_LEFT_MID, 0, 0);

	lv_area_t mask_rect;
	lv_draw_mask_radius_param_t mask_param;

	mask_rect.x1 = 0;
	mask_rect.y1 = 0;
	mask_rect.x2 = DRAWER_WIDTH + CORNER_RADIUS;
	mask_rect.y2 = DRAWER_HEIGHT;
	lv_draw_mask_radius_init(&mask_param, &mask_rect, CORNER_RADIUS, false);
	lv_objmask_add_mask(om, &mask_param);

	/* Preset button 1*/
	const lv_coord_t PRESET_BUTTON_HEIGHT = DRAWER_HEIGHT / 3;

	btn_quick1 = lv_btn_create(om, NULL);
	lv_theme_apply(btn_quick1, (lv_theme_style_t)CUSTOM_THEME_PRESET_BTN);
	lv_obj_set_size(btn_quick1, DRAWER_WIDTH, PRESET_BUTTON_HEIGHT);
	lv_obj_align(btn_quick1, NULL, LV_ALIGN_IN_LEFT_MID, 0, -(PRESET_BUTTON_HEIGHT));

	lv_obj_t *lbl_btn_quick1 = lv_label_create(btn_quick1, NULL);
	lv_label_set_text(lbl_btn_quick1, "350");
	lv_obj_set_event_cb(btn_quick1, btn_quick_event_cb);

	/* Preset button 2*/
	btn_quick2 = lv_btn_create(om, NULL);
	lv_theme_apply(btn_quick2, (lv_theme_style_t)CUSTOM_THEME_PRESET_BTN);
	lv_obj_set_size(btn_quick2, DRAWER_WIDTH, PRESET_BUTTON_HEIGHT);
	lv_obj_align(btn_quick2, NULL, LV_ALIGN_IN_LEFT_MID, 0, 0);

	lv_obj_t *lbl_btn_quick2 = lv_label_create(btn_quick2, NULL);
	lv_label_set_text(lbl_btn_quick2, "300");
	lv_obj_set_event_cb(btn_quick2, btn_quick_event_cb);

	/* Preset button 3 */
	btn_quick3 = lv_btn_create(om, NULL);
	lv_theme_apply(btn_quick3, (lv_theme_style_t)CUSTOM_THEME_PRESET_BTN);
	lv_obj_set_size(btn_quick3, DRAWER_WIDTH, PRESET_BUTTON_HEIGHT);
	lv_obj_align(btn_quick3, NULL, LV_ALIGN_IN_LEFT_MID, 0, PRESET_BUTTON_HEIGHT);

	lv_obj_t *lbl_btn_quick3 = lv_label_create(btn_quick3, NULL);
	lv_label_set_text(lbl_btn_quick3, "250");
	lv_obj_set_event_cb(btn_quick3, btn_quick_event_cb);

	/* Separators */
	const lv_coord_t SEPARATOR_WIDTH = DRAWER_WIDTH - CORNER_RADIUS;
	static lv_point_t line_points[] = {{0, 0}, {0, 0}}; // Must be static because otherwise line will not be drawn
	line_points[1].x = SEPARATOR_WIDTH;					// Must be set after line_points[0] is set because of static initialization

	lv_obj_t *line1 = lv_line_create(om, NULL);
	lv_line_set_points(line1, line_points, 2);
	lv_theme_apply(line1, (lv_theme_style_t)CUSTOM_THEME_PRESET_SEPARATOR);
	lv_obj_align(line1, NULL, LV_ALIGN_IN_LEFT_MID, (CORNER_RADIUS / 2), -(PRESET_BUTTON_HEIGHT / 2));

	lv_obj_t *line2 = lv_line_create(om, NULL);
	lv_line_set_points(line2, line_points, 2);
	lv_theme_apply(line2, (lv_theme_style_t)CUSTOM_THEME_PRESET_SEPARATOR);
	lv_obj_align(line2, NULL, LV_ALIGN_IN_LEFT_MID, (CORNER_RADIUS / 2), (PRESET_BUTTON_HEIGHT / 2));

	/* Animate in */
	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_var(&a, box_presets);
	lv_anim_set_time(&a, ANIMATION_TIME);
	lv_anim_set_delay(&a, 300);
	lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
	lv_anim_set_values(&a, lv_obj_get_x(box_presets) + DRAWER_WIDTH,
					   lv_obj_get_x(box_presets));
	lv_anim_start(&a);

	lv_obj_fade_in(box_presets, ANIMATION_TIME - 50, 300);
}

static void setup_main_screen(void)
{
	/* Power Bar */
	bar_power = lv_bar_create(lv_scr_act(), NULL);
	lv_theme_apply(bar_power, (lv_theme_style_t)CUSTOM_THEME_POWER_BAR);
	lv_obj_set_size(bar_power, POWER_BAR_WIDTH, (LV_VER_RES_MAX - TITLEBAR_BUTTON_HEIGHT - (2 * POWER_BAR_PADDING_VER)));
	lv_bar_set_anim_time(bar_power, 500);
	lv_bar_set_range(bar_power, 0, 20);
	lv_bar_set_value(bar_power, 0, LV_ANIM_ON);
	lv_obj_align(bar_power, NULL, LV_ALIGN_IN_BOTTOM_LEFT, POWER_BAR_PADDING_HOR, -(POWER_BAR_PADDING_VER));

	/* Box Center */
	lv_obj_t *box_center = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_clean_style_list(box_center, LV_OBJ_PART_MAIN);
	lv_obj_set_size(box_center, CENTER_AREA_WIDTH, LV_VER_RES_MAX - TITLEBAR_HEIGHT);
	lv_obj_set_pos(box_center, (POWER_BAR_WIDTH + POWER_BAR_PADDING_HOR), TITLEBAR_HEIGHT);
	// lv_obj_set_style_local_border_width(box_center, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 1);
	// lv_obj_set_style_local_border_color(box_center, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);

	/* Label Iron Status */
	lbl_iron_id = lv_label_create(box_center, NULL);
	lv_label_set_long_mode(lbl_iron_id, LV_LABEL_LONG_CROP);
	lv_label_set_align(lbl_iron_id, LV_LABEL_ALIGN_CENTER);
	lv_obj_set_size(lbl_iron_id, CENTER_AREA_WIDTH, lv_font_roboto_20.line_height);
	lv_label_set_text(lbl_iron_id, "Iron A - T245");
	lv_obj_align(lbl_iron_id, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);
	lv_obj_set_style_local_text_font(lbl_iron_id, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_roboto_20);

	/* Box Temperature */
	box_temperature = lv_obj_create(box_center, NULL);
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
	lv_coord_t w = lv_obj_get_width(lbl_temperature) + lv_obj_get_width(lbl_unit) + (2 * 4);
	lv_obj_set_size(box_temperature, w, 70);
	lv_obj_align(lbl_temperature, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 4, 5);
	lv_obj_align(lbl_unit, lbl_temperature, LV_ALIGN_OUT_RIGHT_TOP, 0, -3);

	/* Align temperature container*/
	lv_obj_align(box_temperature, NULL, LV_ALIGN_CENTER, 0, -10);

	/* Initially hide box temperature */
	lv_obj_set_hidden(box_temperature, true);

	/* Box Sleep */
	box_sleep = lv_obj_create(box_center, NULL);
	lv_obj_clean_style_list(box_sleep, LV_OBJ_PART_MAIN);
	lv_obj_set_size(box_sleep, CENTER_AREA_WIDTH, 120);
	lv_obj_align(box_sleep, NULL, LV_ALIGN_IN_TOP_MID, 0, 37);

	/* Label Iron State Title */
	lbl_iron_state_title = lv_label_create(box_sleep, NULL);
	lv_label_set_long_mode(lbl_iron_state_title, LV_LABEL_LONG_CROP);
	lv_label_set_align(lbl_iron_state_title, LV_LABEL_ALIGN_CENTER);
	lv_obj_set_size(lbl_iron_state_title, CENTER_AREA_WIDTH, 50);
	lv_label_set_text(lbl_iron_state_title, "");
	lv_obj_align(lbl_iron_state_title, NULL, LV_ALIGN_IN_LEFT_MID, 0, -25);
	lv_obj_set_style_local_text_font(lbl_iron_state_title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_roboto_40);
	lv_obj_set_click(lbl_iron_state_title, true);
	lv_obj_set_event_cb(lbl_iron_state_title, main_screen_iron_enable_event_cb);

	/* Label Iron State_MSG1 */
	lbl_iron_state_msg1 = lv_label_create(box_sleep, NULL);
	lv_label_set_long_mode(lbl_iron_state_msg1, LV_LABEL_LONG_CROP);
	lv_label_set_align(lbl_iron_state_msg1, LV_LABEL_ALIGN_CENTER);
	lv_obj_set_size(lbl_iron_state_msg1, CENTER_AREA_WIDTH, 20);
	lv_label_set_text(lbl_iron_state_msg1, "");
	lv_obj_align(lbl_iron_state_msg1, NULL, LV_ALIGN_IN_LEFT_MID, 0, 10);
	lv_obj_set_style_local_text_font(lbl_iron_state_msg1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_roboto_18);

	/* Label Iron State_MSG2 */
	lbl_iron_state_msg2 = lv_label_create(box_sleep, NULL);
	lv_label_set_long_mode(lbl_iron_state_msg2, LV_LABEL_LONG_CROP);
	lv_label_set_align(lbl_iron_state_msg2, LV_LABEL_ALIGN_CENTER);
	lv_obj_set_size(lbl_iron_state_msg2, CENTER_AREA_WIDTH, 20);
	lv_label_set_text(lbl_iron_state_msg2, "");
	lv_obj_align(lbl_iron_state_msg2, NULL, LV_ALIGN_IN_LEFT_MID, 0, 35);
	lv_obj_set_style_local_text_font(lbl_iron_state_msg2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_roboto_18);

	/* Box Setpoint */
	box_setpoint = lv_obj_create(box_center, NULL);
	lv_obj_clean_style_list(box_setpoint, LV_OBJ_PART_MAIN);
	lv_obj_set_size(box_setpoint, CENTER_AREA_WIDTH, 36);
	lv_obj_align(box_setpoint, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -POWER_BAR_PADDING_VER); // Align with power bar

	/* Temperature setpoint label */
	lbl_setpoint = lv_label_create(box_setpoint, NULL);
	lv_label_set_long_mode(lbl_setpoint, LV_LABEL_LONG_CROP);
	lv_obj_set_size(lbl_setpoint, w - 90, lv_font_roboto_28.line_height);
	lv_obj_set_style_local_text_font(lbl_setpoint, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_roboto_28);
	lv_label_set_align(lbl_setpoint, LV_LABEL_ALIGN_CENTER);
	lv_label_set_text(lbl_setpoint, "200°C");
	lv_obj_align(lbl_setpoint, NULL, LV_ALIGN_CENTER, 0, 0);

	/* Button to increase setpoint */
	lv_obj_t *btn_inc_setpoint = lv_btn_create(box_setpoint, NULL);
	lv_obj_set_size(btn_inc_setpoint, 47, 34);
	lv_obj_align(btn_inc_setpoint, lbl_setpoint, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
	lv_theme_apply(btn_inc_setpoint, LV_THEME_SPINBOX_BTN);
	// lv_obj_set_style_local_text_font(btn_inc_setpoint, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_24);
	lv_obj_set_style_local_value_str(btn_inc_setpoint, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_PLUS);
	lv_obj_set_event_cb(btn_inc_setpoint, btn_inc_setpoint_event_cb);

	/* Button to decrease setpoint */
	lv_obj_t *btn_dec_setpoint = lv_btn_create(box_setpoint, NULL);
	lv_obj_set_size(btn_dec_setpoint, 47, 34);
	lv_obj_align(btn_dec_setpoint, lbl_setpoint, LV_ALIGN_OUT_LEFT_MID, 0, 0);
	lv_theme_apply(btn_dec_setpoint, LV_THEME_SPINBOX_BTN);
	// lv_obj_set_style_local_text_font(btn_dec_setpoint, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_24);
	lv_obj_set_style_local_value_str(btn_dec_setpoint, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_MINUS);
	lv_obj_set_event_cb(btn_dec_setpoint, btn_dec_setpoint_event_cb);
}

/* -------------------------------------------------------------------------- */
/*                               EVENT CALLBACKS                              */
/* -------------------------------------------------------------------------- */

LV_EVENT_CB_DECLARE(encoder_target_event_cb)
{
	if (e == LV_EVENT_KEY)
	{
		const uint32_t *key = lv_event_get_data();

		switch (*key)
		{
		case LV_KEY_RIGHT:
			iron_set_setpoint(iron_get_setpoint() + SETPOINT_STEP_SIZE);
			break;
		case LV_KEY_LEFT:
			iron_set_setpoint(iron_get_setpoint() - SETPOINT_STEP_SIZE);
			break;
		}
	}
	else if (e == LV_EVENT_PRESSED)
	{
		// iron_set_enable(!iron_is_enabled());
	}
}

LV_EVENT_CB_DECLARE(btn_inc_setpoint_event_cb)
{
	if (e == LV_EVENT_CLICKED)
		iron_set_setpoint(iron_get_setpoint() + SETPOINT_STEP_SIZE);
}

LV_EVENT_CB_DECLARE(btn_dec_setpoint_event_cb)
{
	if (e == LV_EVENT_CLICKED)
		iron_set_setpoint(iron_get_setpoint() - SETPOINT_STEP_SIZE);
}

static void btn_quick_event_cb(lv_obj_t *obj, lv_event_t event)
{
	switch (event)
	{
	case LV_EVENT_CLICKED:
		if (obj == btn_quick1)
		{
			iron_set_setpoint(350);
		}
		else if (obj == btn_quick2)
		{
			iron_set_setpoint(300);
		}
		else if (obj == btn_quick3)
		{
			iron_set_setpoint(250);
		}
		break;
	default:
		break;
	}
}

static void main_screen_iron_enable_event_cb(lv_obj_t *obj, lv_event_t event)
{
	switch (event)
	{
	case LV_EVENT_VALUE_CHANGED:
		Buzzer_Beep(100);
		iron_set_enable(lv_btn_get_state(obj) == LV_BTN_STATE_CHECKED_RELEASED);
		break;
	case LV_EVENT_CLICKED:
		Buzzer_Beep(100);
		iron_set_enable(!iron_is_enabled());
		break;
	default:
		break;
	}
}

static void main_screen_refresher_task(struct _lv_task_t *taskh)
{
	static uint16_t prev_temperature;
	static uint16_t prev_setpoint;
	static iron_state_t prev_iron_state;
	static int16_t prev_pwr;

	iron_state_t new_iron_state = iron_get_state();
	if (new_iron_state != prev_iron_state || initial_run)
	{
		if (new_iron_state == IRON_STATE_ACTIVE)
		{
			lv_obj_set_hidden(box_temperature, false);
			lv_obj_set_hidden(box_sleep, true);
		}

		if (prev_iron_state == IRON_STATE_ACTIVE)
		{
			lv_obj_set_hidden(box_temperature, true);
			lv_obj_set_hidden(box_sleep, false);
		}

		switch (new_iron_state)
		{
		case IRON_STATE_NOT_CONNECTED:
			lv_label_set_text(lbl_iron_state_title, "No Iron");
			lv_label_set_text(lbl_iron_state_msg1, "Check connection");
			break;
		case IRON_STATE_OFF:
			lv_label_set_text(lbl_iron_state_title, "Disabled");
			lv_label_set_text(lbl_iron_state_msg2, "");
			break;
		case IRON_STATE_SLEEP:
			lv_label_set_text(lbl_iron_state_title, "Sleep");
			break;
		case IRON_STATE_HIBERNATE:
			lv_label_set_text(lbl_iron_state_title, "Hibernate");
			lv_label_set_text(lbl_iron_state_msg2, "");
			break;
		case IRON_STATE_ACTIVE:
			// Nothing to be done on sleep screen
			break;
		}
		prev_iron_state = new_iron_state;
	}

	uint16_t new_temperature = iron_get_temperature();
	if (new_temperature != prev_temperature || initial_run)
	{
		switch (new_iron_state)
		{
		case IRON_STATE_OFF:
		case IRON_STATE_SLEEP:
		case IRON_STATE_HIBERNATE:
			lv_label_set_text_fmt(lbl_iron_state_msg1, "Temperature: %d°C", new_temperature);
			break;
		case IRON_STATE_ACTIVE:
			lv_label_set_text_fmt(lbl_temperature, "%d", new_temperature);
			break;
		default:
			/* No need to update anything */
			break;
		}
		prev_temperature = new_temperature;
	}

	if (new_iron_state == IRON_STATE_SLEEP)
	{
		static uint32_t prev_time_till_hibernate;
		uint32_t new_time_till_hibernate = iron_get_seconds_till_hibernate();
		if (new_time_till_hibernate != prev_time_till_hibernate)
		{
			lv_label_set_text_fmt(lbl_iron_state_msg2, "Hibernate in %d:%02d", new_time_till_hibernate / 60, new_time_till_hibernate % 60);
			prev_time_till_hibernate = new_time_till_hibernate;
		}
	}

	uint16_t new_setpoint = iron_get_setpoint();
	if (new_setpoint != prev_setpoint || initial_run)
	{
		lv_label_set_text_fmt(lbl_setpoint, "%d°C", new_setpoint);
		prev_setpoint = new_setpoint;
	}

	int16_t new_pwr = (int16_t)iron_get_power();
	if (new_pwr != prev_pwr)
	{
		lv_bar_set_value(bar_power, new_pwr, LV_ANIM_OFF);
		prev_pwr = new_pwr;
	}
	initial_run = false;
}

static void switch_to_menu_event_cb(lv_obj_t *obj, lv_event_t event)
{
	if (event == LV_EVENT_CLICKED)
	{
		lv_task_del(main_screen_task_handle);
		lv_obj_del(scr_home);
		scr_menu = create_menu_screen(return_to_home_event_cb);
	}
}
static void return_to_home_event_cb(lv_obj_t *obj, lv_event_t event)
{
	if (event == LV_EVENT_CLICKED)
	{
		lv_obj_del(scr_menu);
		create_main_screen();
	}
}