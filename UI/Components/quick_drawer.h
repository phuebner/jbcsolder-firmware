#ifndef QUICK_DRAWER
#define QUICK_DRAWER

#include "lvgl.h"

lv_obj_t *quick_drawer_create(lv_obj_t *parent);

// Show and hide the quick drawer using animation
lv_anim_t *quick_drawer_show(lv_obj_t *obj);
lv_anim_t *quick_drawer_hide(lv_obj_t *obj);

#endif /* QUICK_DRAWER */
