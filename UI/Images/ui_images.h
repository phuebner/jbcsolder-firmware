#ifndef UI_IMAGES_H
#define UI_IMAGES_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#elif defined(LV_LVGL_H_INCLUDE_SYSTEM)
#include <lvgl.h>
#elif defined(LV_BUILD_TEST)
#include "../lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

// Generated image declarations
extern const lv_image_dsc_t icon_arrow_back_18;

#ifdef __cplusplus
}
#endif

#endif // UI_IMAGES_H
