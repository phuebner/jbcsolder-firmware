/**
 * @file disp.h
 *
 */

#ifndef DRV_LCD
#define DRV_LCD

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include "lv_color.h"
#include "lv_area.h"

/*********************
 *      DEFINES
 *********************/
#define TFT_USE_GPU 0 /*Enable hardware accelerator*/

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

#endif /* DRV_LCD */
