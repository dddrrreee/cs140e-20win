#include <sys/time.h>
#include <sys/resource.h>

#include "libunix.h"

time_usec_t time_get_usec(void) {
    struct timeval t;
    if(gettimeofday(&t, 0))
        sys_die(gettimeofday, "time failed??");
    return t.tv_sec * 1000 * 1000 + t.tv_usec;
}
