// need a better random!!
#include "rpi.h"

// bug in gcc?
// static unsigned initial_seed = 0xACE1u;
#define initial_seed  0xACE1u

static unsigned short lfsr = initial_seed;
static unsigned bit;

unsigned short rpi_rand16(void) {
    bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    lfsr =  (lfsr >> 1) | (bit << 15);
    return lfsr;
}

unsigned long rpi_rand32(void) {
    return (rpi_rand16() << 16) | rpi_rand16();
}

// seed is ignored!  need a better random.
void rpi_reset(unsigned seed) {
    lfsr = initial_seed;
}
