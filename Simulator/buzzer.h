#ifndef BUZZER
#define BUZZER

#include <stdint.h>

// Buzzer initialization function
void Buzzer_Init(void);

// Function to turn the buzzer on
void Buzzer_On(void);

// Function to turn the buzzer off
void Buzzer_Off(void);

// Function to set the buzzer frequency
void Buzzer_SetFrequency(uint16_t frequency);

// Function to set the buzzer volume
void Buzzer_SetVolume(uint8_t volume);

// Function to make the buzzer beep for a specified duration
void Buzzer_Beep(uint16_t duration);

#endif /* BUZZER */
