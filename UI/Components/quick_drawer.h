#ifndef QUICK_DRAWER
#define QUICK_DRAWER

#include "lvgl.h"

#define QUICK_DRAWER_WIDTH 63
#define QUICK_DRAWER_PADDING_VER 14
#define QUICK_DRAWER_RADIUS 10

lv_obj_t *quick_drawer_create(lv_obj_t *parent);

void quick_drawer_set_presets(lv_obj_t *obj, const int presets1, const int presets2, const int presets3);

// Show and hide the quick drawer using animation
lv_anim_t *quick_drawer_show(lv_obj_t *obj);
lv_anim_t *quick_drawer_hide(lv_obj_t *obj);
bool quick_drawer_is_hidden(lv_obj_t *obj);

#endif /* QUICK_DRAWER */
