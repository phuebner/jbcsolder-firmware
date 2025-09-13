/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <SDL2/SDL.h>
#include "lvgl.h"
#include "iron.h"
#include "iron_driver_moc.h"

#include "main_screen.h"

// #include "../Core/Src/drv/lv_port_indev_encoder.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_display_t *hal_init(int32_t w, int32_t h);
static uint32_t zero_crossing_callback(uint32_t interval, void *param);

/**********************
 *  STATIC VARIABLES
 **********************/
iron_t iron_a;
static int half_cycle = 0;
static int cycle = 0;
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

int main(int argc, char **argv)
{
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/

  SDL_Init(SDL_INIT_TIMER);

  /*Initialize LVGL*/
  lv_init();

  /*Initialize the HAL (display, input devices, tick) for LVGL*/
  hal_init(320, 240);
  // hal_lvgl_encoder_init();

  iron_init(&iron_a, &iron_hw_channel_1);

  gui_init();

  SDL_TimerID timerID = SDL_AddTimer(10, zero_crossing_callback, NULL); // 50 Hz voltage zero crossing simulation (zero crossing every 10 ms)

  printf("Soldering Iron Simulator\n");
  printf("Press Ctrl+C to exit\n");
  if (timerID == 0)
  {
    fprintf(stderr, "Failed to create timer: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  uint32_t last_tick = SDL_GetTicks();
  while (1)
  {
    SDL_Delay(5); /*Sleep for 5 milliseconds*/
    uint32_t now = SDL_GetTicks();
    uint32_t elapsed = now - last_tick;
    last_tick = now;      /*Update the last tick time*/
    lv_tick_inc(elapsed); /*Tell LVGL how much time has passed since the last call*/
    lv_timer_handler();
  }

  return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static lv_display_t *hal_init(int32_t w, int32_t h)
{

  lv_group_set_default(lv_group_create());

  lv_display_t *disp = lv_sdl_window_create(w, h);

  lv_indev_t *mouse = lv_sdl_mouse_create();
  lv_indev_set_group(mouse, lv_group_get_default());
  lv_indev_set_display(mouse, disp);
  lv_display_set_default(disp);

  LV_IMAGE_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
  lv_obj_t *cursor_obj;
  cursor_obj = lv_image_create(lv_screen_active()); /*Create an image object for the cursor */
  lv_image_set_src(cursor_obj, &mouse_cursor_icon); /*Set the image source*/
  lv_indev_set_cursor(mouse, cursor_obj);           /*Connect the image  object to the driver*/

  lv_indev_t *mousewheel = lv_sdl_mousewheel_create();
  lv_indev_set_display(mousewheel, disp);
  lv_indev_set_group(mousewheel, lv_group_get_default());

  lv_indev_t *kb = lv_sdl_keyboard_create();
  lv_indev_set_display(kb, disp);
  lv_indev_set_group(kb, lv_group_get_default());

  return disp;
}

static uint32_t zero_crossing_callback(uint32_t interval, void *param)
{
  (void)interval; /*Unused*/
  (void)param;    /*Unused*/
  // This function is called every 10 ms
  // to simulate the zero crossing of the AC mains voltage.
  // It is used to trigger the heater control logic.

  // Track half-cycles and cycles to control the heater in alternating periods
  // half_cycle = half_cycle == 1 ? 0 : 1; // alternate between 0 and 1 to track half cycles
  // cycle += half_cycle;                  // increase cycle counter on every second half cycle
  iron_heater_disable(&iron_a); // Disable the heater before starting a new cycle
  iron_a.drv->adc_start();      // Start ADC conversion to read the temperature
  iron_update_state(&iron_a);
  iron_control_heater(&iron_a); // Control the heater based on the current state and temperature

  simulate_temperature_change(); // Simulate the soldering iron by changing the adc value
  return 1;                      // Return 1 to keep the timer active
}