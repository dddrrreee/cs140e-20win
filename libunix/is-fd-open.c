#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int fd_is_open(int fd) {
    return fcntl(fd, F_GETFL) >= 0;
}
