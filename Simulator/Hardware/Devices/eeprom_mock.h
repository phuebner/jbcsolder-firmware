#ifndef EEPROM_MOCK_H
#define EEPROM_MOCK_H

#include "eeprom.h"

/* -------------------------------------------------------------------------- */
/*                                   DEFINES                                  */
/* -------------------------------------------------------------------------- */

// Mock EEPROM specifications (same as CAT24MD1W for compatibility)
#define EEPROM_MOCK_SIZE_BYTES 512U // 4 Kbit / 8 = 512 bytes
#define EEPROM_MOCK_PAGE_SIZE 16U   // 16-byte page size

/* -------------------------------------------------------------------------- */
/*                              TYPE DEFINITIONS                              */
/* -------------------------------------------------------------------------- */

/**
 * @brief Mock EEPROM configuration structure
 */
typedef struct
{
    const char *storage_file; // Optional file path to persist EEPROM data
    bool simulate_errors;     // Enable error simulation for testing
    uint32_t error_rate;      // Error rate (0-100) for simulated errors
} eeprom_mock_config_t;

/* -------------------------------------------------------------------------- */
/*                           FUNCTION DECLARATIONS                            */
/* -------------------------------------------------------------------------- */

/**
 * @brief Initialize the mock EEPROM driver
 * @param config Configuration structure (can be NULL for default config)
 * @return EEPROM_OK on success, error code otherwise
 */
eeprom_result_t eeprom_mock_init(const eeprom_mock_config_t *config);

/**
 * @brief Get the mock EEPROM driver interface
 * @return Pointer to the driver interface
 */
const eeprom_drv_t *eeprom_mock_get_driver(void);

/**
 * @brief Clear all mock EEPROM data (fill with 0xFF)
 */
void eeprom_mock_clear(void);

/**
 * @brief Get direct access to mock EEPROM data for testing
 * @return Pointer to the internal data buffer
 */
uint8_t *eeprom_mock_get_data(void);

/**
 * @brief Save mock EEPROM data to file (if configured)
 * @return EEPROM_OK on success, error code otherwise
 */
eeprom_result_t eeprom_mock_save_to_file(void);

/**
 * @brief Load mock EEPROM data from file (if configured)
 * @return EEPROM_OK on success, error code otherwise
 */
eeprom_result_t eeprom_mock_load_from_file(void);

#endif /* EEPROM_MOCK_H */
