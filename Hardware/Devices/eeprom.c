/*
 * eeprom.c
 *
 *  Created on: Nov 14, 2025
 *      Author: GitHub Copilot
 */

#include "eeprom.h"

/* -------------------------------------------------------------------------- */
/*                                PRIVATE DATA                                */
/* -------------------------------------------------------------------------- */

static const eeprom_drv_t *g_eeprom_driver = NULL;
static bool g_eeprom_initialized = false;

/* -------------------------------------------------------------------------- */
/*                           FUNCTION DEFINITIONS                             */
/* -------------------------------------------------------------------------- */

const char *eeprom_result_to_string(eeprom_result_t result)
{
    static const char *result_strings[] = {
        [EEPROM_OK] = "OK",
        [EEPROM_ERROR] = "Error",
        [EEPROM_ERROR_TIMEOUT] = "Timeout",
        [EEPROM_ERROR_INVALID_ADDRESS] = "Invalid Address",
        [EEPROM_ERROR_INVALID_SIZE] = "Invalid Size",
        [EEPROM_ERROR_DEVICE_NOT_READY] = "Device Not Ready",
    };

    if (result < sizeof(result_strings) / sizeof(result_strings[0]))
    {
        return result_strings[result];
    }

    return "Unknown Error";
}

eeprom_result_t eeprom_init(const eeprom_drv_t *driver)
{
    if (driver == NULL)
    {
        return EEPROM_ERROR;
    }

    // Store the driver
    g_eeprom_driver = driver;

    // Initialize the driver if it has an init function
    if (g_eeprom_driver->init != NULL)
    {
        eeprom_result_t result = g_eeprom_driver->init();
        if (result != EEPROM_OK)
        {
            g_eeprom_driver = NULL;
            return result;
        }
    }

    g_eeprom_initialized = true;
    return EEPROM_OK;
}

void eeprom_deinit(void)
{
    g_eeprom_driver = NULL;
    g_eeprom_initialized = false;
}

bool eeprom_is_initialized(void)
{
    return g_eeprom_initialized && (g_eeprom_driver != NULL);
}

eeprom_result_t eeprom_read(uint16_t address, uint8_t *data, size_t size)
{
    if (!eeprom_is_initialized())
    {
        return EEPROM_ERROR_DEVICE_NOT_READY;
    }

    if (g_eeprom_driver->read == NULL)
    {
        return EEPROM_ERROR;
    }

    return g_eeprom_driver->read(address, data, size);
}

eeprom_result_t eeprom_write(uint16_t address, const uint8_t *data, size_t size)
{
    if (!eeprom_is_initialized())
    {
        return EEPROM_ERROR_DEVICE_NOT_READY;
    }

    if (g_eeprom_driver->write == NULL)
    {
        return EEPROM_ERROR;
    }

    return g_eeprom_driver->write(address, data, size);
}

uint32_t eeprom_get_size(void)
{
    if (!eeprom_is_initialized() || g_eeprom_driver->get_size == NULL)
    {
        return 0;
    }

    return g_eeprom_driver->get_size();
}

uint16_t eeprom_get_page_size(void)
{
    if (!eeprom_is_initialized() || g_eeprom_driver->get_page_size == NULL)
    {
        return 0;
    }

    return g_eeprom_driver->get_page_size();
}

bool eeprom_is_ready(void)
{
    if (!eeprom_is_initialized())
    {
        return false;
    }

    if (g_eeprom_driver->is_ready == NULL)
    {
        return true; // Assume ready if no ready check function
    }

    return g_eeprom_driver->is_ready();
}