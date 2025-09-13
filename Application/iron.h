/*
 * iron.h
 *
 *  Created on: Dec 16, 2020
 *      Author: patrick
 */

#ifndef SRC_IRON_H_
#define SRC_IRON_H_

#include <stdint.h>
#include <stdbool.h>
#include "pid.h"

/* -------------------------------------------------------------------------- */
/*                              TYPE DEFINITIONS                              */
/* -------------------------------------------------------------------------- */

typedef struct
{
    uint16_t sleep_temperature;
    uint32_t hibernate_delay; /** Hibernate delay in number of halfwaves (10ms @ 50Hz or  8.33ms @ 60Hz) */
} iron_config_t;

/**
 * @brief Driver interface for soldering iron hardware
 *
 * This structure defines function pointers for controlling the soldering iron hardware.
 * It provides interfaces to enable or disable the amplifier and heater, start ADC conversion,
 * and read ADC values.
 */
typedef struct
{
    void (*amplifier_en)(bool enable);
    void (*heater_en)(bool enable);
    void (*adc_start)(void);
    uint16_t (*adc_read)(void);
    _Bool (*get_sleep_pin_state)(void); /** Get the state of the sleep pin */
} iron_drv_t;

typedef enum
{
    IRON_STATE_NOT_CONNECTED,
    IRON_STATE_OFF,
    IRON_STATE_HIBERNATE,
    IRON_STATE_SLEEP,
    IRON_STATE_ACTIVE
} iron_state_t;

// typedef void (*iron_state_change_cb_t)(const iron_state_t state);

typedef struct
{
    iron_config_t cfg;        /** Configuration of the soldering iron */
    iron_drv_t *drv;          /** Hardware driver for the soldering iron */
    uint16_t setpoint;        /** Setpoint temperature in degrees Celsius */
    float temperature;        /** Current temperature value */
    float temperature_smooth; /** Smoothed temperature value */
    iron_state_t state;       /** Current state of the soldering iron */
    _Bool enabled;            /** Whether the soldering iron is enabled or not */
    uint32_t hibernate_timer; /** Hibernate timer counts up till it reaches cfg.hibernate_delay */
    PIDControl pid;
    uint16_t power;
    float pid_out;
    uint32_t half_cycle_counter;      /** Heater half cycle counter for controlling the heater in alternating periods */
    uint32_t heater_skip_half_cycles; /** Number of half cycles to skip for the heater control, 0xFFFF means no power, 0 means full power */
    // iron_state_change_cb_t state_change_cb;
} iron_t;

/* -------------------------------------------------------------------------- */
/*                              GLOBAL PROTOTYPS                              */
/* -------------------------------------------------------------------------- */

/**
 * @brief Init instance of soldering iron
 *
 */
void iron_init(iron_t *iron, iron_drv_t *drv);

/* ---------------------------- Setter Functions ---------------------------- */

void iron_set_setpoint(iron_t *iron, uint16_t temperature);
void iron_set_enable(iron_t *iron, _Bool value);
// void iron_set_state_change_cb(iron_state_change_cb_t cb);

/* ---------------------------- Getter Functions ---------------------------- */

uint16_t iron_get_temperature(iron_t *iron);
uint16_t iron_get_setpoint(iron_t *iron);
float iron_get_power(iron_t *iron);
iron_state_t iron_get_state(iron_t *iron);
uint32_t iron_get_seconds_till_hibernate(iron_t *iron);

_Bool iron_is_enabled(iron_t *iron);
_Bool iron_is_sleeping(iron_t *iron);

/* ---------------------------- Other Functions ----------------------------- */
void iron_heater_disable(iron_t *iron);
void iron_control_heater(iron_t *iron);
void iron_update_state(iron_t *iron);

#endif /* SRC_IRON_H_ */
