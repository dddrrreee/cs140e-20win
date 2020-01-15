#include <string.h>

// https://clc-wiki.net/wiki/strncmp#Implementation
int strncmp(const char* _s1, const char* _s2, size_t n) {
    const unsigned char *s1 = (void*)_s1, *s2 = (void*)_s2;
    while(n--) {
        if(*s1++!=*s2++)
            return s1[-1] - s2[-1];
    }
    return 0;
}
