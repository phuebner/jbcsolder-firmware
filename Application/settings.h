#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>
#include <stdbool.h>

#define SETTINGS_VERSION 1

enum settings_id_e
{
    SETTINGS_ID_BUZZER = 0,
    // Add more setting IDs as needed
    SETTINGS_ID_LENGTH
};

typedef enum
{
    SETTING_TYPE_UINT32,
    SETTING_TYPE_INT32,
    SETTING_TYPE_FLOAT,
    SETTING_TYPE_BOOL,
    SETTING_TYPE_STRING,
} settings_type_t;

typedef struct
{
    const uint16_t id;          // Unique identifier for the setting
    const char *name;           // Name of the setting
    const settings_type_t type; // Type of the setting
    uint32_t min;               // Minimum value
    uint32_t max;               // Maximum value
    uint32_t default_value;     // Default value
} settings_definition_t;

void load_settings(void);
void save_settings(void);

void settings_set_bool(uint16_t id, bool value);
void settings_set_uint32(uint16_t id, uint32_t value);

bool settings_get_bool(uint16_t id);
uint32_t settings_get_uint32(uint16_t id);

void print_settings(void);
#endif // SETTINGS_H