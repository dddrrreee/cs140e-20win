#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "rpi.h"

int printk(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
        printf("PI:");
        int res = vfprintf(stdout, fmt, args);
        fflush(stdout);
    va_end(args);
    return res;
}

