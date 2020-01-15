#include <string.h>

void *memset(void *_p, int c, size_t n) {
    char *p = _p, *e = p + n;

    while(p < e)
        *p++ = c;
    return _p;
}
