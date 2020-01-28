#include <assert.h>
#include "libunix.h"

int can_read_timeout(int fd, unsigned usec) {
    unimplemented();
}

// doesn't block.
int can_read(int fd) { return can_read_timeout(fd, 0); }
