#include <stdarg.h>

#include "rpi.h"
#include "va-printk.h"

int printk(const char *fmt, ...) {
    // this is a really huge buffer for a pi.   could do this better.
    static char buf[1024];
    va_list args;

    va_start(args, fmt);
        int sz = va_printk(buf, sizeof buf, fmt, args);
    va_end(args);


#if 1
    putk(buf);
#else
    int i;
    for(i = 0; buf[i]; i++)
        rpi_putchar(buf[i]);
#endif
    return sz;
}
