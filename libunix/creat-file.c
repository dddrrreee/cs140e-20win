#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "libunix.h"

int create_file(const char *name) {
    // XXX: without O_TRUNC can get really messed up.
    int fd = open(name, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
    if(fd < 0)
        sys_die(open, "could not open file: <%s>", name);
    return fd;
}
