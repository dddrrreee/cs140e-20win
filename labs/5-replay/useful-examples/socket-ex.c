// engler: Example using socket, fork, waitpid.    similar to pipe-ex.

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>

#include "demand.h"

// just echo what the child sends, make sure waitpid() can give us exit statis.
void parent(int fd, int pid) {
	int ret;

	// echo whever child sent.
	unsigned val = 0;
	while(val < 10) {
		if((ret = read(fd, &val, 4)) != 4)
			panic("read should not fail\n");
		printf("Parent: child sent val=%d\n", val);
		val++;
		printf("Parent: sending val=%d\n", val);
		if(write(fd, &val, 4) != 4)
			panic("write should not fail\n");
	}
	printf("Parent done with loop\n");

	// read() of closed socket = EOF, which means ret == 0.  ret < 0
	// is some other error.
	if((ret = read(fd, &val, 4)) != 0)
		panic("expected EOF, got: %d\n", ret);

	// Block til child dies, print exit code.
	int status;
	if(waitpid(pid, &status, 0) < 0)
		sys_die(waitpid, should never fail);

	    printf("Parent waiting for child: ");
        if(!WIFEXITED(status))
                printf("Child crashed.\n");
        else {
                status = WEXITSTATUS(status);
                printf("Child exited with status = %d\n", status);
		assert(status == 10);
	}
	printf("SUCCESS!\n");

	printf("about to write to closed socket\n");

    // 2. probably clearer: ignore the signal.
    if(signal(SIGPIPE, SIG_IGN) < 0)
        sys_die(signal, cannot catch);

	if(write(fd, &val, 4) < 0)
		fprintf(stderr, "broken pip\n");

	fprintf(stderr, "successx2\n");
	fprintf(stderr, "successx2\n");
	fprintf(stderr, "successx2\n");
	fprintf(stderr, "successx2\n");
}

void child(int fd) {
	int ret;
	unsigned val = 0;
	while(val < 10) {
		printf("\tchild sending val=%d\n", val);
		if((ret = write(fd, &val, 4)) != 4)
			panic("write should not fail, expected 4 got: %d\n", ret);
		if((ret = read(fd, &val, 4)) != 4)
			panic("read should not fail, expected 4 got %d\n", ret);
		printf("\tchild received val=%d\n", val);
	}
	printf("child done with loop\n");
	exit(10);
}

int main(void) {
    int sock[2];
    if(socketpair(PF_LOCAL, SOCK_STREAM, 0, sock) < 0)
        sys_die(socketpair, failed);

	int pid;
	if((pid = fork()) < 0)
		sys_die(fork, fork failed);

	if(pid != 0) {
		if(close(sock[1]) < 0)
			sys_die(close, impossible);
		parent(sock[0], pid);
	} else {
		if(close(sock[0]) < 0)
			sys_die(close, close failed?);
		child(sock[1]);
	} 
	return 0;
}
