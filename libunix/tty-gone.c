#include <sys/stat.h>
#include "libunix.h"

int tty_gone(const char *ttyname) {
    struct stat s;
    return stat(ttyname, &s) < 0; 
}
