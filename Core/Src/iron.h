/*
 * iron.h
 *
 *  Created on: Dec 16, 2020
 *      Author: patrick
 */

#ifndef SRC_IRON_H_
#define SRC_IRON_H_

#include "main.h"
#include "pid.h"

/* -------------------------------------------------------------------------- */
/*                              TYPE DEFINITIONS                              */
/* -------------------------------------------------------------------------- */

typedef struct {
    uint16_t sleep_temperature;
    uint32_t hibernate_delay; /** Hibernate delay in number of halfwaves (10ms @ 50Hz or  8.33ms @ 60Hz) */
} iron_config_t;

typedef enum {
    IRON_STATE_NOT_CONNECTED,
    IRON_STATE_OFF,
    IRON_STATE_HIBERNATE,
    IRON_STATE_SLEEP,
    IRON_STATE_ACTIVE
} iron_state_t;

// typedef void (*iron_state_change_cb_t)(const iron_state_t state);

typedef struct {
    iron_config_t cfg;
    uint16_t setpoint;
    float temperature;
    iron_state_t state;
    _Bool enabled;
    uint32_t hibernate_timer; /** Hibernate timer counts up till it reaches cfg.hibernate_delay */
    // iron_state_change_cb_t state_change_cb;
} iron_t;

/* -------------------------------------------------------------------------- */
/*                              GLOBAL PROTOTYPS                              */
/* -------------------------------------------------------------------------- */

/**
 * @brief Init instance of soldering iron 
 * 
 */
void iron_init();

/* ---------------------------- Setter Functions ---------------------------- */

void iron_set_setpoint(uint16_t temperature);
void iron_set_enable(_Bool value);
// void iron_set_state_change_cb(iron_state_change_cb_t cb);

/* ---------------------------- Getter Functions ---------------------------- */

uint16_t iron_get_temperature();
uint16_t iron_get_setpoint();
float iron_get_power();
iron_state_t iron_get_state();
uint32_t iron_get_seconds_till_hibernate();

_Bool iron_is_enabled();
_Bool iron_is_sleeping();

/* -------------------------------------------------------------------------- */
/*                             INTERRUPT HANDLERS                             */
/* -------------------------------------------------------------------------- */

void iron_timer_irq_handler();
void iron_zero_cross_irq_handler();

#endif /* SRC_IRON_H_ */
