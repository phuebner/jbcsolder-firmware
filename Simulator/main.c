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

#include "simulation_control_screen.h"

#include "iron.h"
#include "iron_driver_moc.h"
#include "main_screen.h"
#include "input_devices.h"

#include "eeprom.h"
#include "eeprom_mock.h"

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
lv_display_t *disp_main;
lv_display_t *disp_sim_ctl;

static void hal_init(int32_t w, int32_t h);
static uint32_t zero_crossing_callback(uint32_t interval, void *param);

/**********************
 *  STATIC VARIABLES
 **********************/
iron_t *iron_a;

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
  InputDevices_Init();

  // Initialize mock EEPROM for simulator
  eeprom_mock_config_t mock_config = {
      .storage_file = "eeprom_data.bin", // Persist data to file
      .simulate_errors = false,          // Disable error simulation by default
      .error_rate = 0                    // 0% error rate
  };

  eeprom_result_t result = eeprom_mock_init(&mock_config);
  if (result != EEPROM_OK)
  {
    printf("Failed to initialize mock EEPROM: %s\n", eeprom_result_to_string(result));
    return result;
  }

  const eeprom_drv_t *driver = eeprom_mock_get_driver();

  // Now initialize the EEPROM management system with the driver
  result = eeprom_init(driver);
  if (result != EEPROM_OK)
  {
    printf("Failed to initialize EEPROM management system: %s\n", eeprom_result_to_string(result));
    return result;
  }

  printf("EEPROM initialized successfully\n");
  printf("EEPROM size: %lu bytes\n", (unsigned long)eeprom_get_size());
  printf("Page size: %u bytes\n", eeprom_get_page_size());

  iron_a = iron_init(IRON_IDENTIFIER_A, &iron_hw_channel_1, "Iron A", IRON_TYPE_JBC_T245);

  lv_display_set_default(disp_sim_ctl);
  simulation_control_screen_create();
  lv_display_set_default(disp_main);

  gui_init();

  SDL_TimerID timerID = SDL_AddTimer(10, zero_crossing_callback, NULL); // 50 Hz voltage zero crossing simulation (zero crossing every 10 ms)
  if (timerID == 0)
  {
    fprintf(stderr, "Failed to create timer: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  printf("Soldering Iron Simulator\n");
  printf("Press Ctrl+C to exit\n");

  while (1)
  {
    SDL_Delay(5); /*Sleep for 5 milliseconds*/

    static uint32_t last_tick = 0;
    uint32_t now = SDL_GetTicks64();
    if (last_tick > now)
    {
      last_tick = now; // Handle tick overflow
      continue;
    }

    lv_tick_inc(now - last_tick); /*Tell LVGL how much time has passed since the last call*/
    lv_timer_handler();
    last_tick = now;
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
static void hal_init(int32_t w, int32_t h)
{

  lv_group_set_default(lv_group_create());

  // Create the display windows
  // The order here matters for setting the default display
  disp_sim_ctl = lv_sdl_window_create(w, h);
  disp_main = lv_sdl_window_create(w, h);

  lv_display_set_default(disp_main);

  /* -------- Set up position of the two windows and handle exit events ------- */
  SDL_Window *window_disp_main = lv_sdl_window_get_window(disp_main);
  SDL_Window *window_disp_sim_ctl = lv_sdl_window_get_window(disp_sim_ctl);
  int x, y;
  SDL_GetWindowPosition(window_disp_main, &x, &y);
  SDL_SetWindowPosition(window_disp_sim_ctl, x + 350, y);
}

/**
 * Timer callback to simulate zero crossing of AC mains voltage
 */
static uint32_t zero_crossing_callback(uint32_t interval, void *param)
{
  (void)interval; /*Unused*/
  (void)param;    /*Unused*/

  if (iron_a == NULL)
    return 10; // Return to call this function again in 10 ms

  // This function is called every 10 ms
  // to simulate the zero crossing of the AC mains voltage.
  // It is used to trigger the heater control logic.

  // Track half-cycles and cycles to control the heater in alternating periods
  // half_cycle = half_cycle == 1 ? 0 : 1; // alternate between 0 and 1 to track half cycles
  // cycle += half_cycle;                  // increase cycle counter on every second half cycle
  iron_heater_disable(iron_a); // Disable the heater before starting a new cycle
  iron_start_adc(iron_a);      // Start ADC conversion to read the temperature
  iron_update_state(iron_a);
  iron_control_heater(iron_a); // Control the heater based on the current state and temperature

  simulate_temperature_change(); // Simulate the soldering iron by changing the adc value
  return 10;                     // Call this function again in 10 ms
}