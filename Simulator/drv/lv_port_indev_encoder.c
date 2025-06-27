#include "lv_port_indev_encoder.h"

lv_indev_t *enc_indev;

void lv_port_indev_encoder_init()
{
	enc_indev = lv_indev_create();
	lv_indev_set_type(enc_indev, LV_INDEV_TYPE_ENCODER);
}
