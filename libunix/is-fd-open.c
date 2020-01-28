#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int is_fd_open(int fd) {
    return fcntl(fd, F_GETFL) >= 0;
}
