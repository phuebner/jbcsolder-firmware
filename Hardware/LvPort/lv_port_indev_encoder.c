#include "lv_port_indev_encoder.h"
#include "main.h"
#include "tim.h"

/* -------------------------------------------------------------------------- */
/*                         Static Function Definitions                        */
/* -------------------------------------------------------------------------- */

static void _encoder_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    LV_UNUSED(indev);
    int32_t enc_diff = ((int16_t)(htim1.Instance->CNT)) / 2;
    if (enc_diff != 0)
    {
        htim1.Instance->CNT = 0; // reset counter
    }

    data->enc_diff = enc_diff;
    data->state = (HAL_GPIO_ReadPin(BUTTON_ENC_GPIO_Port, BUTTON_ENC_Pin) == GPIO_PIN_SET) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}

/* -------------------------------------------------------------------------- */
/*                         Public Function Definitions                        */
/* -------------------------------------------------------------------------- */

lv_indev_t *lv_port_indev_encoder_init()
{
    lv_indev_t *enc_indev = lv_indev_create();
    lv_indev_set_type(enc_indev, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(enc_indev, _encoder_read_cb);
    return enc_indev;
}
