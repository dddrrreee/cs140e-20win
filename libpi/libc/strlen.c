#include <string.h>

size_t strlen(const char *p) {
    size_t ret;
    for (ret = 0; p[ret]; ++ret)
        ;
    return ret;
}
