// need a better random!!
#include "rpi.h"

static unsigned short lfsr = 0xACE1u;
static unsigned bit;

unsigned short rpi_rand16(void) {
    bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    lfsr =  (lfsr >> 1) | (bit << 15);
    return lfsr;
}

unsigned rpi_rand32(void) {
    return (rpi_rand16() << 16) | rpi_rand16();
}

