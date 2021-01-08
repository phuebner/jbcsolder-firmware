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
    float sleept_temperature;
} iron_config_t;

typedef enum {
    IRON_STATE_OFF,
    IRON_STATE_HIBERNATE,
    IRON_STATE_SLEEP,
    IRON_STATE_ACTIVE
} iron_state_t;

// typedef void (*iron_state_change_cb_t)(const iron_state_t state);

typedef struct {
    iron_config_t cfg;
    float setpoint;
    float temperature;
    iron_state_t state;
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

_Bool iron_is_enabled();
_Bool iron_is_sleeping();

/* -------------------------------------------------------------------------- */
/*                             INTERRUPT HANDLERS                             */
/* -------------------------------------------------------------------------- */

void iron_timer_irq_handler();
void iron_zero_cross_irq_handler();

#endif /* SRC_IRON_H_ */
