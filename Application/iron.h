/*
 * iron.h
 *
 *  Created on: Dec 16, 2020
 *      Author: patrick
 */

#ifndef IRON
#define IRON

#include <stdint.h>
#include <stdbool.h>
#include "pid.h"
/* -------------------------------------------------------------------------- */
/*                                   DEFINES                                  */
/* -------------------------------------------------------------------------- */
#define IRON_MAX_OBSERVERS 4 // Maximum number of observers for the iron state changes

/* -------------------------------------------------------------------------- */
/*                              TYPE DEFINITIONS                              */
/* -------------------------------------------------------------------------- */
typedef enum
{
    IRON_IDENTIFIER_A = 0,
    IRON_IDENTIFIER_B,
    IRON_COUNT,
} ironIdentifier_e;

typedef struct _iron_t iron_t; // Forward declaration of iron_t for use in observer callback typedef

typedef void (*iron_observer_cb_t)(iron_t *iron, void *user_data); // Callback type for iron state change observers

typedef struct
{
    iron_observer_cb_t cb;
    void *user_data;
} iron_observer_t;

typedef struct
{
    uint16_t sleep_temperature;
    uint32_t hibernate_delay; /** Hibernate delay in number of half-waves (10ms @ 50Hz or  8.33ms @ 60Hz) */
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

// States of the soldering iron
typedef enum
{
    IRON_STATE_NOT_CONNECTED,
    IRON_STATE_OFF,
    IRON_STATE_HIBERNATE,
    IRON_STATE_SLEEP,
    IRON_STATE_ACTIVE,
} iron_state_t;

// String representations of the iron states for UI display and logging
static const char *state_str[] = {
    [IRON_STATE_NOT_CONNECTED] = "Not Connected",
    [IRON_STATE_OFF] = "Disabled",
    [IRON_STATE_HIBERNATE] = "Hibernate",
    [IRON_STATE_SLEEP] = "Sleep",
    [IRON_STATE_ACTIVE] = "Active",
};

// Types of supported soldering irons
typedef enum
{
    IRON_TYPE_UNKNOWN,
    IRON_TYPE_JBC_T245,
    IRON_TYPE_JBC_T210,
} iron_type_t;

// String representations of the iron types for UI display and logging
static const char *iron_type_str[] = {
    [IRON_TYPE_UNKNOWN] = "Unknown",
    [IRON_TYPE_JBC_T245] = "JBC T245",
    [IRON_TYPE_JBC_T210] = "JBC T210",
};

// typedef void (*iron_state_change_cb_t)(const iron_state_t state);

struct _iron_t
{
    char name[20];            /** Name of the soldering iron */
    iron_type_t type;         /** Type of the soldering iron */
    iron_config_t cfg;        /** Configuration of the soldering iron */
    iron_drv_t *drv;          /** Hardware driver for the soldering iron */
    uint16_t setpoint;        /** Setpoint temperature in degrees Celsius */
    float temperature;        /** Current temperature value */
    float temperature_smooth; /** Smoothed temperature value */
    iron_state_t state;       /** Current state of the soldering iron */
    _Bool enabled;            /** Whether the soldering iron is enabled or not */
    uint32_t hibernate_timer; /** Hibernate timer counts up till it reaches cfg.hibernate_delay */
    uint16_t power;

    /* Heater control */
    PIDControl pid;
    float pid_out;
    int16_t pos_on_count;
    int16_t neg_on_count;
    int16_t accumulator;
};
/* -------------------------------------------------------------------------- */
/*                              GLOBAL PROTOTYPES                              */
/* -------------------------------------------------------------------------- */

/**
 * @brief Initializes an iron instance with the provided configuration.
 *
 * @param iron Pointer to the iron instance to be initialized
 * @param drv Pointer to the iron driver configuration
 * @param name String identifier for the iron instance
 * @param type Type identifier for the iron instance
 *
 * @note This function must be called before using any other iron-related functions
 */
iron_t *iron_init(ironIdentifier_e identifier, iron_drv_t *drv, char *name, iron_type_t type);

/**
 *  Add an observer callback to the soldering iron instance
 *
 * @param iron  Pointer to the soldering iron instance
 * @param cb    Observer callback function
 * @return uint8_t  New number of observers, or 0 if the maximum is reached
 */
// uint8_t iron_observer_add(iron_t *iron, iron_observer_cb_t cb, void *user_data);

/**
 *  Remove an observer callback from the soldering iron instance
 *
 * @param iron  Pointer to the soldering iron instance
 * @param cb    Observer callback function to remove
 * @return uint8_t  1 if successfully removed, 0 if not found
 */
// uint8_t iron_observer_remove(iron_t *iron, iron_observer_cb_t cb);

/* ---------------------------- Setter Functions ---------------------------- */

void iron_set_setpoint(iron_t *iron, uint16_t temperature);
void iron_set_enable(iron_t *iron, _Bool value);

/* ---------------------------- Getter Functions ---------------------------- */

uint16_t iron_get_temperature(iron_t *iron);
uint16_t iron_get_setpoint(iron_t *iron);
float iron_get_power(iron_t *iron);
iron_state_t iron_get_state(iron_t *iron);
uint32_t iron_get_seconds_till_hibernate(iron_t *iron);

_Bool iron_is_enabled(iron_t *iron);
_Bool iron_is_sleeping(iron_t *iron);

/* ---------------------------- Other Functions ----------------------------- */
static inline void iron_heater_disable(iron_t *iron)
{
    iron->drv->heater_en(false);
}

static inline void iron_enable_amplifier(iron_t *iron, _Bool enable)
{
    iron->drv->amplifier_en(enable);
}
static inline void iron_start_adc(iron_t *iron)
{
    iron->drv->adc_start();
}

void iron_control_heater(iron_t *iron);
void iron_update_state(iron_t *iron);

#endif /* IRON */
