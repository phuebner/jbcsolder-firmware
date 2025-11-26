
#ifndef EEPROM
#define EEPROM

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* -------------------------------------------------------------------------- */
/*                                   DEFINES                                  */
/* -------------------------------------------------------------------------- */

typedef enum
{
    EEPROM_OK = 0,
    EEPROM_ERROR,
    EEPROM_ERROR_TIMEOUT,
    EEPROM_ERROR_INVALID_ADDRESS,
    EEPROM_ERROR_INVALID_SIZE,
    EEPROM_ERROR_DEVICE_NOT_READY,
} eeprom_result_t;

/* -------------------------------------------------------------------------- */
/*                              TYPE DEFINITIONS                              */
/* -------------------------------------------------------------------------- */

/**
 * @brief Abstract EEPROM driver interface
 *
 * This structure defines function pointers for EEPROM operations.
 */
typedef struct
{
    /**
     * @brief Initialize the EEPROM device
     * @return EEPROM_OK on success, error code otherwise
     */
    eeprom_result_t (*init)(void);

    /**
     * @brief Read data from EEPROM
     * @param address Memory address to read from
     * @param data Pointer to buffer to store read data
     * @param size Number of bytes to read
     * @return EEPROM_OK on success, error code otherwise
     */
    eeprom_result_t (*read)(uint16_t address, uint8_t *data, size_t size);

    /**
     * @brief Write data to EEPROM
     * @param address Memory address to write to
     * @param data Pointer to data to write
     * @param size Number of bytes to write
     * @return EEPROM_OK on success, error code otherwise
     */
    eeprom_result_t (*write)(uint16_t address, const uint8_t *data, size_t size);

    /**
     * @brief Get the total size of the EEPROM in bytes
     * @return Size in bytes
     */
    uint32_t (*get_size)(void);

    /**
     * @brief Get the page size for write operations
     * @return Page size in bytes
     */
    uint16_t (*get_page_size)(void);

    /**
     * @brief Check if EEPROM is ready for operations
     * @return true if ready, false otherwise
     */
    bool (*is_ready)(void);

} eeprom_drv_t;

/* -------------------------------------------------------------------------- */
/*                           FUNCTION DECLARATIONS                            */
/* -------------------------------------------------------------------------- */

/**
 * @brief Initialize the EEPROM management system with a specific driver
 * @param driver Pointer to the EEPROM driver implementation
 * @return EEPROM_OK on success, error code otherwise
 */
eeprom_result_t eeprom_init(const eeprom_drv_t *driver);

/**
 * @brief Deinitialize the EEPROM management system
 */
void eeprom_deinit(void);

/**
 * @brief Check if EEPROM system is initialized
 * @return true if initialized, false otherwise
 */
bool eeprom_is_initialized(void);

/**
 * @brief Read data from EEPROM using the initialized driver
 * @param address Memory address to read from
 * @param data Pointer to buffer to store read data
 * @param size Number of bytes to read
 * @return EEPROM_OK on success, error code otherwise
 */
eeprom_result_t eeprom_read(uint16_t address, uint8_t *data, size_t size);

/**
 * @brief Write data to EEPROM using the initialized driver
 * @param address Memory address to write to
 * @param data Pointer to data to write
 * @param size Number of bytes to write
 * @return EEPROM_OK on success, error code otherwise
 */
eeprom_result_t eeprom_write(uint16_t address, const uint8_t *data, size_t size);

/**
 * @brief Get the total size of the EEPROM in bytes
 * @return Size in bytes, or 0 if not initialized
 */
uint32_t eeprom_get_size(void);

/**
 * @brief Get the page size for write operations
 * @return Page size in bytes, or 0 if not initialized
 */
uint16_t eeprom_get_page_size(void);

/**
 * @brief Check if EEPROM is ready for operations
 * @return true if ready, false if not ready or not initialized
 */
bool eeprom_is_ready(void);

/**
 * @brief Convert EEPROM result code to string
 * @param result Result code
 * @return String representation of the result
 */
const char *eeprom_result_to_string(eeprom_result_t result);

#endif /* EEPROM */
