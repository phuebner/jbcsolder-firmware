/*
 * cat24md1w.h
 *
 *  Created on: Nov 14, 2025
 *      Author: GitHub Copilot
 */

#ifndef EEPROM_CAT24MD1W
#define EEPROM_CAT24MD1W

#include "eeprom.h"

/* -------------------------------------------------------------------------- */
/*                                   DEFINES                                  */
/* -------------------------------------------------------------------------- */

// CAT24MD1W specifications
#define CAT24MD1W_SIZE_BYTES 512U      // 4 Kbit / 8 = 512 bytes
#define CAT24MD1W_PAGE_SIZE 16U        // 16-byte page size
#define CAT24MD1W_ADDRESS_MASK 0x01FFU // 9-bit address (512 bytes)
#define CAT24MD1W_WRITE_CYCLE_TIME 5U  // Maximum write cycle time in ms

// I2C configuration
#define CAT24MD1W_I2C_ADDRESS 0x50U // Base I2C address (7-bit)
#define CAT24MD1W_I2C_TIMEOUT 100U  // I2C timeout in ms

/* -------------------------------------------------------------------------- */
/*                              TYPE DEFINITIONS                              */
/* -------------------------------------------------------------------------- */

/**
 * @brief CAT24MD1W configuration structure
 */
typedef struct
{
    void *i2c_handle;       // Pointer to I2C handle (I2C_HandleTypeDef*)
    uint8_t device_address; // Full 8-bit I2C device address (including R/W bit shifted)
} cat24md1w_config_t;

/* -------------------------------------------------------------------------- */
/*                           FUNCTION DECLARATIONS                            */
/* -------------------------------------------------------------------------- */

/**
 * @brief Initialize the CAT24MD1W EEPROM driver
 * @param config Configuration structure
 * @return EEPROM_OK on success, error code otherwise
 */
eeprom_result_t cat24md1w_init(const cat24md1w_config_t *config);

/**
 * @brief Get the CAT24MD1W EEPROM driver interface
 * @return Pointer to the driver interface
 */
const eeprom_drv_t *cat24md1w_get_driver(void);

#endif /* EEPROM_CAT24MD1W */
