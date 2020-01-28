
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>

#ifdef  __APPLE__
#   include <util.h>
#else
#   include <pty.h>
#endif

#include "libunix.h"
int open_fake_tty(char **dev_name) {
    char name[1024];

    int master_fd, 
            slave_fd;   // unused.
    if(openpty(&master_fd, &slave_fd, name, 0, 0))
        sys_die(openpty, failed!);

#if 0
    // XXX: check if this causes <name> to be deleted.
    close_nofail(slave_fd);
#endif

    debug("fake ptty=<%s>, master=%d,slave=%d\n", name, master_fd, slave_fd);
    *dev_name = strdup(name);
    return master_fd;
}
