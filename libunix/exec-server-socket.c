#include <unistd.h>
#include <sys/socket.h>

#include "libunix.h"

// fork exec server, create a socket for comparison and dup it to the <to_fd>
int exec_server_socket(int *pid, char **argv, int to_fd) {

    debug("going to fork/exec: %s ", argv[0]);
    for(int i = 1; argv[i]; i++)
        fprintf(stderr, " argv[%d]=<%s> ", i, argv[i]);
    fprintf(stderr, "\n");
    unimplemented();
}
