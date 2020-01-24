// engler: if a process writes to a closed pipe, will get a SIGPIPE.
// this shows two ways to handle this:
//	1. you can ignore the signal explicitly.
//	2. you could also not close the write end of the pipe in the parent,
// 	so if the child exits, you don't get an error.  This is probably less
//	clean.

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "demand.h"

// if(signal(SIGPIPE, catch_sigpipe) < 0)
void catch_sigpipe(int sig) {
	fprintf(stderr, "caught an epipe!\n");
}

int main(void) {
	int fds[2];

	// fds[0] = read fd, fds[1] = write fd
	if(pipe(fds) < 0)
		sys_die(pipe, cannot create pipe fds);

	// try removing the fork().  what is going on?
	int pid;
	if(!(pid=fork())) {
		// exit will do this, obviously.  but for non-trivial programs
		// it's good practice.
		close(fds[0]);
		exit(0);
	}

	// XXX: To see SIGPIPE, comment out (2); to then make it go away,
	// comment out (1).

	// 1. Close the parent read end.
	if(close(fds[0]) < 0)
		sys_die(close, cannot close?);

	// prevent race condition. ["what?"]
	int status;
	if(waitpid(pid, &status, 0) < 0)
		sys_die(waitpid, should never fail);

#if 0
	// 2. probably clearer: ignore the signal.
	if(signal(SIGPIPE, SIG_IGN) < 0)
		sys_die(signal, cannot catch);
#endif

	printf("about to write to closed pipe\n");
	char c;
	int ret;
	if((ret = write(fds[1], &c, 1)) == 1)
		// if you commented out (1) & (2) what is going on?
		printf("should have failed!\n");
	printf("ret=%d\n", ret);
	return 0;
}
