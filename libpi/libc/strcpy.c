#include <string.h>

char *strcpy(char * s1, const char * s2) {
    register char *s = s1;
    while ( (*s++ = *s2++) != 0 );
    return s1;
}
