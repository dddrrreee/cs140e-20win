#include <stdarg.h>
#include <stdlib.h>
#include "libunix.h"

// should grab the output?
void run_system(const char *fmt, ...) {
    char buf[4096];

    va_list args;
    va_start(args, fmt);
        vsprintf(buf, fmt, args);
    va_end(args);
    int res;
    if((res = system(buf)) != 0)
        panic("system <%s> failed with %d\n", buf, res);
    debug("SUCCESS: child is done running <%s>!\n", buf);
}
