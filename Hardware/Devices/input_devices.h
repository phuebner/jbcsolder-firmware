/**
 * @file input_devices.h
 * @brief Global input device declarations for LVGL simulator
 *
 * This header provides access to global input device objects that are
 * initialized in main.c and can be used throughout the application.
 */

#pragma once

#include "lvgl.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /* -------------------------------------------------------------------------- */
    /*                              Global variables                              */
    /* -------------------------------------------------------------------------- */
    extern lv_indev_t *mouse_indev; /**< Main window mouse input device */
    extern lv_indev_t *enc_indev;   /**< Encoder (mousewheel) input device */
#ifdef SIMULATOR
    extern lv_indev_t *kb_indev;            /**< Keyboard input device */
    extern lv_indev_t *mouse_sim_ctl_indev; /**< Simulation control window mouse input device */
#endif

    void InputDevices_Init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif
