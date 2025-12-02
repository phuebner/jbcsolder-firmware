#include "settings.h"
#include "eeprom.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define SETTINGS_START_ADDRESS 0x0100 // EEPROM start address for settings
#define SETTINGS_MAGIC 0xA5A5A5A5     // Magic number for settings validation

/**
 * @brief Settings storage structure
 */
typedef struct
{
    uint32_t magic;
    uint32_t version;
    uint32_t size;
    uint32_t data[SETTINGS_ID_LENGTH];
} settings_store_t;

/** Settings storage instance */
static settings_store_t settings_store;

/** Settings definitions list */
static const settings_definition_t settings_list[] = {
    {.id = SETTINGS_ID_BUZZER, .name = "buzzer", .type = SETTING_TYPE_BOOL, .default_value = 0, .min = 0, .max = 1},
    // Add more settings as needed
};

/**
 * @brief Sanitize the settings store by validating and initializing settings
 */
static void _sanitize_settings_store(void)
{
    bool dirty = false;
    // Check magic and version
    if (settings_store.magic != SETTINGS_MAGIC || settings_store.version != SETTINGS_VERSION)
    {
        memset(&settings_store, 0, sizeof(settings_store_t));
        settings_store.magic = SETTINGS_MAGIC;
        settings_store.version = SETTINGS_VERSION;
        dirty = true;
    }

    // Check if new settings have been added at the end
    if (settings_store.size < (int)SETTINGS_ID_LENGTH)
    {
        dirty = true;
        // Initialize new settings to default values
        for (uint16_t i = settings_store.size; i < (uint16_t)SETTINGS_ID_LENGTH; i++)
        {
            const settings_definition_t *def = &settings_list[i];
            settings_store.data[def->id] = def->default_value;
        }
        settings_store.size = (int)SETTINGS_ID_LENGTH;
    }

    // Validate each setting against its min/max
    for (uint16_t i = 0; i < (uint16_t)SETTINGS_ID_LENGTH; i++)
    {
        const settings_definition_t *def = &settings_list[i];

        if (settings_store.data[def->id] < def->min || settings_store.data[def->id] > def->max)
        {
            settings_store.data[def->id] = def->default_value;
            dirty = true;
        }
    }

    // Save if any changes were made
    if (dirty)
    {
        save_settings();
    }
}

/* -------------------------------------------------------------------------- */
/*                                   Public                                   */
/* -------------------------------------------------------------------------- */

void load_settings(void)
{
    memset(&settings_store, 0, sizeof(settings_store_t));
    eeprom_read(SETTINGS_START_ADDRESS, (uint8_t *)&settings_store, sizeof(settings_store_t));
    _sanitize_settings_store();
}

void save_settings(void)
{
    eeprom_write(SETTINGS_START_ADDRESS, (uint8_t *)&settings_store, sizeof(settings_store_t));
}

void settings_set_bool(uint16_t id, bool value)
{
    assert(id < SETTINGS_ID_LENGTH);
    assert(settings_list[id].type == SETTING_TYPE_BOOL);

    settings_store.data[id] = value ? 1 : 0;
}

void settings_set_uint32(uint16_t id, uint32_t value)
{
    assert(id < SETTINGS_ID_LENGTH);
    assert(settings_list[id].type == SETTING_TYPE_UINT32);

    settings_store.data[id] = value;
}
inline uint32_t settings_get_uint32(uint16_t id)
{
    assert(id < SETTINGS_ID_LENGTH);
    assert(settings_list[id].type == SETTING_TYPE_UINT32);

    return settings_store.data[id];
}

bool settings_get_bool(uint16_t id)
{
    assert(id < SETTINGS_ID_LENGTH);
    assert(settings_list[id].type == SETTING_TYPE_BOOL);

    return (settings_store.data[id] != 0);
}

void print_settings(void)
{
    for (size_t i = 0; i < sizeof(settings_list) / sizeof(settings_definition_t); i++)
    {
        const settings_definition_t *def = &settings_list[i];
        uint32_t value = settings_store.data[def->id];
        printf("Setting %s (ID: %d) = %u\n", def->name, def->id, value);
    }
}