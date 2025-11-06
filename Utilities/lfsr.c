/**
 * @file    lfsr.c
 * @brief   Lightweight pseudo-random generator using LFSR (Linear Feedback Shift Register).
 *
 * HOW IT WORKS:
 * -------------
 * An LFSR is a deterministic shift register that generates a sequence of pseudo-random numbers
 * using bitwise operations (shift + XOR feedback). It is ideal for use in interrupt routines or
 * firmware where:
 *   - Speed is critical (<1 µs per call)
 *   - No dynamic memory or complex math is allowed
 *   - A repeatable pseudo-random sequence is acceptable
 *
 * This implementation:
 *   - Uses a 16-bit maximal-length polynomial: x^16 + x^14 + x^13 + x^11 + 1 (0xB400)
 *   - Produces a sequence of 65,535 non-zero values before repeating
 *   - Requires **no initialization function** — starts automatically from seed 0xACE1
 *   - Is safe to call directly inside a zero-crossing ISR to generate random delay (jitter)
 *
 * IMPORTANT NOTES:
 *   - The internal state must never be zero (state = 0 would lock the LFSR permanently).
 *   - You can replace the default seed (0xACE1) if you want a different pseudo-random sequence.
 *   - NOT cryptographically secure — intended for dithering, jitter, noise-like behavior.
 *
 * Example Usage (in ISR for zero-cross detection):
 * ------------------------------------------------
 * uint16_t r = lfsr_next();
 * uint16_t delay_us = 50 + (r % 451);   // random delay between 50 and 500 µs
 * arm_gate_timer(delay_us);
 */

#include "lfsr.h"

// Internal 16-bit LFSR state (must never be 0). Defaults to non-zero seed.
static uint16_t lfsr_state = 0xACE1u;

uint16_t lfsr_next(void)
{
    // Extract least significant bit
    uint16_t lsb = lfsr_state & 1;

    // Shift right by 1 bit
    lfsr_state >>= 1;

    // If bit was 1, apply polynomial feedback
    if (lsb)
        lfsr_state ^= 0xB400u; // taps: 16, 14, 13, 11

    return lfsr_state;
}