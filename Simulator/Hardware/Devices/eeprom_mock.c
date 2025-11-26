/*
 * eeprom_mock.c
 *
 *  Created on: Nov 14, 2025
 *      Author: GitHub Copilot
 */

#include "eeprom_mock.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* -------------------------------------------------------------------------- */
/*                                PRIVATE DATA                                */
/* -------------------------------------------------------------------------- */

static uint8_t g_eeprom_data[EEPROM_MOCK_SIZE_BYTES];
static eeprom_mock_config_t g_config;
static bool g_initialized = false;
static bool g_device_ready = true;

/* -------------------------------------------------------------------------- */
/*                          PRIVATE FUNCTION DECLARATIONS                     */
/* -------------------------------------------------------------------------- */

static eeprom_result_t eeprom_mock_init_impl(void);
static eeprom_result_t eeprom_mock_read_impl(uint16_t address, uint8_t *data, size_t size);
static eeprom_result_t eeprom_mock_write_impl(uint16_t address, const uint8_t *data, size_t size);
static uint32_t eeprom_mock_get_size_impl(void);
static uint16_t eeprom_mock_get_page_size_impl(void);
static bool eeprom_mock_is_ready_impl(void);

static bool should_simulate_error(void);

/* -------------------------------------------------------------------------- */
/*                               DRIVER INTERFACE                             */
/* -------------------------------------------------------------------------- */

static const eeprom_drv_t eeprom_mock_driver = {
    .init = eeprom_mock_init_impl,
    .read = eeprom_mock_read_impl,
    .write = eeprom_mock_write_impl,
    .get_size = eeprom_mock_get_size_impl,
    .get_page_size = eeprom_mock_get_page_size_impl,
    .is_ready = eeprom_mock_is_ready_impl,
};

/* -------------------------------------------------------------------------- */
/*                           FUNCTION DEFINITIONS                             */
/* -------------------------------------------------------------------------- */

eeprom_result_t eeprom_mock_init(const eeprom_mock_config_t *config)
{
    // Initialize with default values
    memset(&g_config, 0, sizeof(eeprom_mock_config_t));

    if (config != NULL)
    {
        memcpy(&g_config, config, sizeof(eeprom_mock_config_t));
    }

    // Initialize EEPROM data to 0xFF (typical EEPROM erased state)
    memset(g_eeprom_data, 0xFF, sizeof(g_eeprom_data));

    // Initialize random seed for error simulation
    srand((unsigned int)time(NULL));

    g_initialized = true;

    // Try to load from file if configured
    if (g_config.storage_file != NULL)
    {
        eeprom_mock_load_from_file(); // Ignore errors, just use default data
    }

    return EEPROM_OK;
}

const eeprom_drv_t *eeprom_mock_get_driver(void)
{
    return &eeprom_mock_driver;
}

void eeprom_mock_clear(void)
{
    memset(g_eeprom_data, 0xFF, sizeof(g_eeprom_data));
}

uint8_t *eeprom_mock_get_data(void)
{
    return g_eeprom_data;
}

eeprom_result_t eeprom_mock_save_to_file(void)
{
    if (!g_initialized || g_config.storage_file == NULL)
    {
        return EEPROM_ERROR;
    }

    FILE *file = fopen(g_config.storage_file, "wb");
    if (file == NULL)
    {
        return EEPROM_ERROR;
    }

    size_t written = fwrite(g_eeprom_data, 1, sizeof(g_eeprom_data), file);
    fclose(file);

    return (written == sizeof(g_eeprom_data)) ? EEPROM_OK : EEPROM_ERROR;
}

eeprom_result_t eeprom_mock_load_from_file(void)
{
    if (!g_initialized || g_config.storage_file == NULL)
    {
        return EEPROM_ERROR;
    }

    FILE *file = fopen(g_config.storage_file, "rb");
    if (file == NULL)
    {
        return EEPROM_ERROR;
    }

    size_t read_count = fread(g_eeprom_data, 1, sizeof(g_eeprom_data), file);
    fclose(file);

    return (read_count == sizeof(g_eeprom_data)) ? EEPROM_OK : EEPROM_ERROR;
}

/* -------------------------------------------------------------------------- */
/*                        PRIVATE FUNCTION DEFINITIONS                        */
/* -------------------------------------------------------------------------- */

static eeprom_result_t eeprom_mock_init_impl(void)
{
    if (!g_initialized)
    {
        return EEPROM_ERROR;
    }

    if (should_simulate_error())
    {
        return EEPROM_ERROR_DEVICE_NOT_READY;
    }

    return EEPROM_OK;
}

static eeprom_result_t eeprom_mock_read_impl(uint16_t address, uint8_t *data, size_t size)
{
    if (!g_initialized || data == NULL || size == 0)
    {
        return EEPROM_ERROR;
    }

    // Check address bounds
    if (address >= EEPROM_MOCK_SIZE_BYTES || (address + size) > EEPROM_MOCK_SIZE_BYTES)
    {
        return EEPROM_ERROR_INVALID_ADDRESS;
    }

    if (!g_device_ready)
    {
        return EEPROM_ERROR_DEVICE_NOT_READY;
    }

    if (should_simulate_error())
    {
        return EEPROM_ERROR;
    }

    // Copy data from mock EEPROM
    memcpy(data, &g_eeprom_data[address], size);

    return EEPROM_OK;
}

static eeprom_result_t eeprom_mock_write_impl(uint16_t address, const uint8_t *data, size_t size)
{
    if (!g_initialized || data == NULL || size == 0)
    {
        return EEPROM_ERROR;
    }

    // Check address bounds
    if (address >= EEPROM_MOCK_SIZE_BYTES || (address + size) > EEPROM_MOCK_SIZE_BYTES)
    {
        return EEPROM_ERROR_INVALID_ADDRESS;
    }

    if (!g_device_ready)
    {
        return EEPROM_ERROR_DEVICE_NOT_READY;
    }

    if (should_simulate_error())
    {
        return EEPROM_ERROR;
    }

    // Simulate page-wise writing (like real EEPROM)
    size_t bytes_written = 0;

    while (bytes_written < size)
    {
        uint16_t current_address = address + bytes_written;

        // Calculate how many bytes we can write in this page
        uint16_t page_offset = current_address % EEPROM_MOCK_PAGE_SIZE;
        uint16_t bytes_to_write = EEPROM_MOCK_PAGE_SIZE - page_offset;

        // Don't write more than requested
        if (bytes_to_write > (size - bytes_written))
        {
            bytes_to_write = size - bytes_written;
        }

        // Copy data to mock EEPROM
        memcpy(&g_eeprom_data[current_address], &data[bytes_written], bytes_to_write);

        bytes_written += bytes_to_write;

        // Simulate write delay (optional)
        // In a real implementation, this might be handled by the calling code
    }

    // Auto-save to file if configured
    if (g_config.storage_file != NULL)
    {
        eeprom_mock_save_to_file(); // Ignore errors
    }

    return EEPROM_OK;
}

static uint32_t eeprom_mock_get_size_impl(void)
{
    return EEPROM_MOCK_SIZE_BYTES;
}

static uint16_t eeprom_mock_get_page_size_impl(void)
{
    return EEPROM_MOCK_PAGE_SIZE;
}

static bool eeprom_mock_is_ready_impl(void)
{
    if (!g_initialized)
    {
        return false;
    }

    return g_device_ready;
}

static bool should_simulate_error(void)
{
    if (!g_config.simulate_errors || g_config.error_rate == 0)
    {
        return false;
    }

    uint32_t random_value = rand() % 100;
    return (random_value < g_config.error_rate);
}