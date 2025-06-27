#ifndef IRON_HW_CHANNEL
#define IRON_HW_CHANNEL

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    void (*amplifier_en)(bool enable);
    void (*heater_en)(bool enable);
    void (*adc_start)(void);
    uint16_t (*adc_read)(void);
} iron_hw_channel_t;

extern iron_hw_channel_t iron_hw_channel_1;
extern iron_hw_channel_t iron_hw_channel_2;

#endif /* IRON_HW_CHANNEL */
