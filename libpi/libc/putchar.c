#include "rpi.h"

static int internal_putchar(int c) { uart_putc(c); return c; }

// override this is going through the network.
int (*rpi_putchar)(int c) = internal_putchar;
