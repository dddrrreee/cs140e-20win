#include <string.h>

// from uclibc
char *strchr(register const char *s, int c) {
    do {
        if (*s == ((char)c)) {
            return (char *) s; /* silence the warning */
        }
    } while (*s++);

    return NULL;
}

