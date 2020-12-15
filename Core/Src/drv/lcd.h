/**
 * @file disp.h
 * 
 */

#ifndef DISP_H
#define DISP_H

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include "lvgl/src/lv_misc/lv_color.h"
#include "lvgl/src/lv_misc/lv_area.h"

/*********************
 *      DEFINES
 *********************/
#define TFT_USE_GPU		0		/*Enable hardware accelerator*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void tft_init(void);

/**********************
 *      MACROS
 **********************/

#endif
