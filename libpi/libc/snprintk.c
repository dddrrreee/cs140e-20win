#include <stdarg.h>
#include "rpi.h"
#include "va-printk.h"

int snprintk(char *buf, size_t n, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
        int sz = va_printk(buf, n, fmt, args);
    va_end(args);
    return sz;
}
