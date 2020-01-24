#include "rpi.h"

// much faster to send this as an entire string using the esp.
int internal_putk(const char *p) {
    for(; *p; p++)
        rpi_putchar(*p);
//  we were adding this.
//  rpi_putchar('\n');
    return 1;
}

int (*putk)(const char *p) = internal_putk;
