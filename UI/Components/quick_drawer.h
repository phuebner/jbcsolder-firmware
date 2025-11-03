#ifndef QUICK_DRAWER
#define QUICK_DRAWER

#include "lvgl.h"

static const lv_coord_t QUICK_DRAWER_WIDTH = 63;
static const lv_coord_t QUICK_DRAWER_PADDING_VER = 14;

lv_obj_t *quick_drawer_create(lv_obj_t *parent);

// Show and hide the quick drawer using animation
lv_anim_t *quick_drawer_show(lv_obj_t *obj);
lv_anim_t *quick_drawer_hide(lv_obj_t *obj);

#endif /* QUICK_DRAWER */
