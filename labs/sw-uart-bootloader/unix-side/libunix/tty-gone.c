// engler, cs140e.
#include <sys/stat.h>
#include "libunix.h"

// hack to see if the ttyusb serial was unplugged.
int tty_gone(const char *ttyname) {
    struct stat s;
    return stat(ttyname, &s) < 0; 
}
