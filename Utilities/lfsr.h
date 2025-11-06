#ifndef LFSR
#define LFSR

#include <stdint.h>

// Returns next 32-bit pseudo-random number (1..0xFFFFFFFF)
// No initialization required — safe to call anywhere (ISR/loop/etc).
uint16_t lfsr_next(void);

#endif /* LFSR */
