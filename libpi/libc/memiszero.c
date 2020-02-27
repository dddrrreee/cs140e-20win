#include "rpi.h"

int memiszero(const void *_p, unsigned n) {
    const char *p = _p;
    for(int i = 0; i < n; i++)
        if(p[i] != 0)
            return 0;
    return 1;
}

