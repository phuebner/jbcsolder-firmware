#include "input_devices.h"

lv_indev_t *mouse_indev = NULL;
lv_indev_t *enc_indev = NULL;
lv_indev_t *kb_indev = NULL;
lv_indev_t *mouse_sim_ctl_indev = NULL;

void InputDevices_Init(void)
{

    mouse_indev = lv_sdl_mouse_create();
    enc_indev = lv_sdl_mousewheel_create();
    kb_indev = lv_sdl_keyboard_create();

    lv_display_t *disp_main = lv_display_get_default();
    lv_display_t *disp_sim_ctl = lv_display_get_next(disp_main); // Assuming the second display is the sim control
    mouse_sim_ctl_indev = lv_sdl_mouse_create();
    lv_indev_set_display(mouse_sim_ctl_indev, disp_sim_ctl);
    lv_indev_set_group(mouse_sim_ctl_indev, lv_group_get_default());
}
