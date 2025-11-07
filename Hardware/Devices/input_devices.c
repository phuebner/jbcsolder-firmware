#include "input_devices.h"
#include "main.h"
#include "tim.h"
#include "lv_port_indev_touchscreen.h"
#include "lv_port_indev_encoder.h"

/* -------------------------------------------------------------------------- */
/*                              Global variables                              */
/* -------------------------------------------------------------------------- */
lv_indev_t *mouse_indev = NULL;
lv_indev_t *enc_indev = NULL;

void InputDevices_Init(void)
{

    mouse_indev = lv_port_indev_touchscreen_init();
    enc_indev = lv_port_indev_encoder_init();

    HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
}