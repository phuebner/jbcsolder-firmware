/*
 * ft6236u.c
 *
 *  Created on: Dec 15, 2020
 *      Author: patrick
 */

#include "ft6236u.h"
#include "main.h"
#include "i2c.h"

#define FT6236_I2C_ADDRESS (0x38 << 1)
#define FT6236_ORIENTATION_HORIZONTAL 1

#define FT6236_REG_DATA 0x00

static touch_event_t last_event;
static void record_event();

void ft6236_touch_io_init()
{
	// Release from reset
	HAL_GPIO_WritePin(TS_RST_GPIO_Port, TS_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(300);
	uint8_t res[1];
	HAL_I2C_Mem_Read(&hi2c1, FT6236_I2C_ADDRESS, 0xA3, I2C_MEMADD_SIZE_8BIT, res, 1, 1000);
}

touch_event_t ft6236_touch_get_last_event()
{
	return last_event;
}

void ft6236_touch_interrupt_handler()
{
	record_event();
}

/**
 * @brief Records the latest touch event from the FT6236 touch controller.
 *
 * This function reads touch data from the FT6236 controller over I2C and
 * processes it to extract touch event details such as type, coordinates,
 * weight, and area. The extracted event is stored in the `last_event` variable
 * for later retrieval.
 */
static void record_event()
{
	// Returns a list of touchpoint dicts, with 'x' and 'y' containing the
	// touch coordinates, and 'id' as the touch # for multitouch tracking

	uint8_t data[8];
	HAL_I2C_Mem_Read(&hi2c1, FT6236_I2C_ADDRESS, FT6236_REG_DATA, I2C_MEMADD_SIZE_8BIT, data, 8, 1000);

	// Check if the touch is valid
	touch_event_t event;
	event.type = data[3] >> 6;
#if FT6236_ORIENTATION_HORIZONTAL
	event.x = 320 - (((data[5] & 0x07) << 8) | (data[6]));
	event.y = ((data[3] & 0x07) << 8) | (data[4]);
#else
	event.x = ((data[3] & 0x07) << 8) | (data[4]);
	event.y = ((data[5] & 0x07) << 8) | (data[6]);
#endif
	event.weight = data[7];
	event.area = data[8];
	last_event = event;
}
