#ifndef FT6236U_FT6236U
#define FT6236U_FT6236U
#include "main.h"

typedef enum
{
	TOUCH_EVENT_PRESSED = 0,
	TOUCH_EVENT_REL = 1,
	TOUCH_EVENT_CONTACT = 2,
	TOUCH_EVENT_NONE = 3,
} touch_event_type_t;

typedef struct
{
	touch_event_type_t type;
	uint16_t x;
	uint16_t y;
	uint8_t weight;
	uint8_t area;
} touch_event_t;

void touch_init();

touch_event_t touch_get_last_event();

void touch_interrupt_handler();

#endif /* FT6236U_FT6236U */
