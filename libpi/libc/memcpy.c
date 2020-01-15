#include <string.h>

#define aligned(ptr, n)  ((unsigned)ptr % n == 0)
#define aligned4(ptr)  aligned(ptr,4)

void *memcpy(void *dst, const void *src, size_t nbytes) { 
    // this is not for optimization.   when gcc copies structs it may
    // call memcpy.   if the dst struct is a pointer to hw, and we
    // do byte stores, i don't think this will necessarily lead to 
    // good behavior.
    if(aligned4(dst) && aligned4(src) && aligned4(nbytes)) {
        unsigned n = nbytes / 4;
        unsigned *d = dst;
        const unsigned *s = src;

        for(unsigned i = 0; i < n; i++)
            d[i] = s[i];
    } else {
        unsigned char *d = dst;
        const unsigned char *s = src;
        for(unsigned i = 0; i < nbytes; i++)
            d[i] = s[i];
    }
    return dst;
}
