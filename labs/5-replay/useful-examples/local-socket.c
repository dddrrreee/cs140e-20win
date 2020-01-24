// engler,cs140e: shows how to use a local socket.  these are bi-directional, 
// so you only need one file descriptor.
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "demand.h"

// simple asynchronous read implementation: will return bytes of data read,
// or if blocked for <timeout_secs>, 0.
int async_read(int fd, void *buf, unsigned sz, unsigned timeout_secs) {
    struct timeval tv;
    fd_set rfds;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
    tv.tv_sec = timeout_secs;
    tv.tv_usec = 0;

    int r;
    if((r = select(fd+1, &rfds, NULL, NULL, &tv)) < 0)
        sys_die(select, failed?);
    else if(!r)
        return 0;

    assert(FD_ISSET(fd, &rfds));
    return read(fd, buf, sz);
}

static int exit_code(int pid) {
    int status;
    if(waitpid(pid, &status, 0) < 0)
        sys_die("waitpid", waitpid failed?);
    if(!WIFEXITED(status))
        panic("%d: unexpected crash\n", pid);
    return WEXITSTATUS(status);
}

void fork_echo(void) {
	int fds[2];
	if(socketpair(PF_LOCAL, SOCK_STREAM, 0, fds) < 0)
		sys_die(socketpair, failed);

	int pid;
	if((pid = fork()) < 0)
		sys_die(fork, should not fail);
	if(!pid) {
		char msg[1024];
		sprintf(msg, "got hello from pid=%d", getpid());
		if(write(fds[1], msg, strlen(msg)+1) < 0)
			sys_die(write, write failed);
		if(async_read(fds[1], msg, sizeof msg, 0) != 0)
			sys_die(read, read should return 0 bytes);
		sleep(1);
		exit(0);
	}
	char buf[1024];
	if(read(fds[0], buf, sizeof buf) < 0)
		sys_die(read, read failed);
	printf("parent: got <%s> from child\n", buf);
	printf("parent: child exited with: <%d>\n", exit_code(pid));

	unsigned char b = 1;
	int r = write(fds[0], &b, 1);
	if(r < 0)
		sys_die(write, write to closed socket failed);
	printf("write to closed socket = %d bytes\n", r);
}

int main(void) {
	for(int i = 0; i < 10; i++)
		fork_echo();
	return 0;
}
