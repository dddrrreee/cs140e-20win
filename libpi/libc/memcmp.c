#include <stdlib.h>

int memcmp(const void *_s1, const void *_s2, size_t nbytes) {
    const unsigned char *s1 = _s1, *s2 = _s2;

    for(int i = 0; i < nbytes; i++) {
        int v = s1[i] - s2[i];
        if(v)
            return v;
    }
    return 0;
}
