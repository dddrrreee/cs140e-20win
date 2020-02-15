#include <unistd.h>
#include "libunix.h"

int read_timeout(int fd, void *data, unsigned n, unsigned timeout) {
    if(!can_read_timeout(fd, timeout))
        return 0;
    if((n = read(fd, data, n)) < 0)
        sys_die(read, read failed);
    return n;
}
