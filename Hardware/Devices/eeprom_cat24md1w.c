/*
 * cat24md1w.c
 *
 *  Created on: Nov 14, 2025
 *      Author: GitHub Copilot
 */

#include "eeprom_cat24md1w.h"
#include "i2c.h"
#include "stm32f7xx_hal.h"
#include <string.h>

/* -------------------------------------------------------------------------- */
/*                                PRIVATE DATA                                */
/* -------------------------------------------------------------------------- */

static cat24md1w_config_t g_config;
static bool g_initialized = false;

/* -------------------------------------------------------------------------- */
/*                          PRIVATE FUNCTION DECLARATIONS                     */
/* -------------------------------------------------------------------------- */

static eeprom_result_t cat24md1w_init_impl(void);
static eeprom_result_t cat24md1w_read_impl(uint16_t address, uint8_t *data, size_t size);
static eeprom_result_t cat24md1w_write_impl(uint16_t address, const uint8_t *data, size_t size);
static uint32_t cat24md1w_get_size_impl(void);
static uint16_t cat24md1w_get_page_size_impl(void);
static bool cat24md1w_is_ready_impl(void);

static eeprom_result_t cat24md1w_wait_for_write_complete(void);
static eeprom_result_t cat24md1w_hal_to_eeprom_result(HAL_StatusTypeDef hal_status);

/* -------------------------------------------------------------------------- */
/*                               DRIVER INTERFACE                             */
/* -------------------------------------------------------------------------- */

static const eeprom_drv_t cat24md1w_driver = {
    .init = cat24md1w_init_impl,
    .read = cat24md1w_read_impl,
    .write = cat24md1w_write_impl,
    .get_size = cat24md1w_get_size_impl,
    .get_page_size = cat24md1w_get_page_size_impl,
    .is_ready = cat24md1w_is_ready_impl,
};

/* -------------------------------------------------------------------------- */
/*                           FUNCTION DEFINITIONS                             */
/* -------------------------------------------------------------------------- */

eeprom_result_t cat24md1w_init(const cat24md1w_config_t *config)
{
    if (config == NULL || config->i2c_handle == NULL)
    {
        return EEPROM_ERROR;
    }

    // Store configuration
    memcpy(&g_config, config, sizeof(cat24md1w_config_t));
    g_initialized = true;

    return EEPROM_OK;
}

const eeprom_drv_t *cat24md1w_get_driver(void)
{
    return &cat24md1w_driver;
}

/* -------------------------------------------------------------------------- */
/*                        PRIVATE FUNCTION DEFINITIONS                        */
/* -------------------------------------------------------------------------- */

static eeprom_result_t cat24md1w_init_impl(void)
{
    if (!g_initialized)
    {
        return EEPROM_ERROR;
    }

    // Check if device is present by trying to read device address
    HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(
        (I2C_HandleTypeDef *)g_config.i2c_handle,
        g_config.device_address,
        3, // 3 trials
        CAT24MD1W_I2C_TIMEOUT);

    return (status == HAL_OK) ? EEPROM_OK : EEPROM_ERROR_DEVICE_NOT_READY;
}

static eeprom_result_t cat24md1w_read_impl(uint16_t address, uint8_t *data, size_t size)
{
    if (!g_initialized || data == NULL || size == 0)
    {
        return EEPROM_ERROR;
    }

    // Check address bounds
    if (address >= CAT24MD1W_SIZE_BYTES || (address + size) > CAT24MD1W_SIZE_BYTES)
    {
        return EEPROM_ERROR_INVALID_ADDRESS;
    }

    // CAT24MD1W uses 8-bit address, but we need to handle it as a memory address
    // For addresses > 255, we need to use the next I2C device address
    uint8_t device_addr = g_config.device_address;
    uint8_t mem_addr = (uint8_t)(address & 0xFF);

    // Handle address overflow by using A16 bit in device address
    if (address > 0xFFFF)
    {
        device_addr |= 0x02; // Set A16 bit
    }

    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
        (I2C_HandleTypeDef *)g_config.i2c_handle,
        device_addr,
        mem_addr,
        I2C_MEMADD_SIZE_16BIT,
        data,
        size,
        CAT24MD1W_I2C_TIMEOUT);

    return cat24md1w_hal_to_eeprom_result(status);
}

static eeprom_result_t cat24md1w_write_impl(uint16_t address, const uint8_t *data, size_t size)
{
    if (!g_initialized || data == NULL || size == 0)
    {
        return EEPROM_ERROR;
    }

    // Check address bounds
    if (address >= CAT24MD1W_SIZE_BYTES || (address + size) > CAT24MD1W_SIZE_BYTES)
    {
        return EEPROM_ERROR_INVALID_ADDRESS;
    }

    size_t bytes_written = 0;
    eeprom_result_t result = EEPROM_OK;

    while (bytes_written < size && result == EEPROM_OK)
    {
        uint16_t current_address = address + bytes_written;

        // Calculate how many bytes we can write in this page
        uint16_t page_offset = current_address % CAT24MD1W_PAGE_SIZE;
        uint16_t bytes_to_write = CAT24MD1W_PAGE_SIZE - page_offset;

        // Don't write more than requested
        if (bytes_to_write > (size - bytes_written))
        {
            bytes_to_write = size - bytes_written;
        }

        // Determine device address based on memory address
        uint8_t device_addr = g_config.device_address;
        uint8_t mem_addr = (uint8_t)(current_address & 0xFF);

        // Handle address overflow by using A16 bit in device address
        if (current_address > 0xFFFF)
        {
            device_addr |= 0x02; // Set A16 bit
        }

        // Write the page
        HAL_StatusTypeDef status = HAL_I2C_Mem_Write(
            (I2C_HandleTypeDef *)g_config.i2c_handle,
            device_addr,
            mem_addr,
            I2C_MEMADD_SIZE_16BIT,
            (uint8_t *)(data + bytes_written),
            bytes_to_write,
            CAT24MD1W_I2C_TIMEOUT);

        if (status != HAL_OK)
        {
            result = cat24md1w_hal_to_eeprom_result(status);
            break;
        }

        // Wait for write cycle to complete
        result = cat24md1w_wait_for_write_complete();
        if (result != EEPROM_OK)
        {
            break;
        }

        bytes_written += bytes_to_write;
    }

    return result;
}

static uint32_t cat24md1w_get_size_impl(void)
{
    return CAT24MD1W_SIZE_BYTES;
}

static uint16_t cat24md1w_get_page_size_impl(void)
{
    return CAT24MD1W_PAGE_SIZE;
}

static bool cat24md1w_is_ready_impl(void)
{
    if (!g_initialized)
    {
        return false;
    }

    HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(
        (I2C_HandleTypeDef *)g_config.i2c_handle,
        g_config.device_address,
        1, // 1 trial
        10 // Short timeout
    );

    return (status == HAL_OK);
}

static eeprom_result_t cat24md1w_wait_for_write_complete(void)
{
    uint32_t start_time = HAL_GetTick();

    while ((HAL_GetTick() - start_time) < CAT24MD1W_WRITE_CYCLE_TIME)
    {
        if (cat24md1w_is_ready_impl())
        {
            return EEPROM_OK;
        }
        HAL_Delay(1);
    }

    return EEPROM_ERROR_TIMEOUT;
}

static eeprom_result_t cat24md1w_hal_to_eeprom_result(HAL_StatusTypeDef hal_status)
{
    switch (hal_status)
    {
    case HAL_OK:
        return EEPROM_OK;
    case HAL_TIMEOUT:
        return EEPROM_ERROR_TIMEOUT;
    default:
        return EEPROM_ERROR;
    }
}