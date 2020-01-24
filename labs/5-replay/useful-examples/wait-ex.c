// engler, cs140e: Different ways to wait for children.
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "demand.h"

static int make_kid(int exit_code) {
	int pid;
	if((pid = fork()) < 0)
		sys_die(fork, fork failed?);
	// child
	if(!pid) {
		debug("forked kid=%d\n", getpid());
		sleep(1);
		exit(exit_code);
	}
	return pid;
}

int main(void) {
	int status, exit_code = 10;

	int pid = make_kid(exit_code);
	debug("testing <waitpid(pid, &status, 0)>\n");
	if(waitpid(pid, &status, 0) < 0)
		sys_die(waitpid, waitpid failed?);
	status = WEXITSTATUS(status);
	debug("\tkid=%d exited with %d\n\n", pid, status);
	assert(status == exit_code);
	exit_code++;

	// should be equiv.
	pid = make_kid(exit_code);
	debug("testing <waitpid(-1, &status, 0)>\n");
	if(waitpid(-1, &status, 0) != pid)
		sys_die(waitpid, returned different pid);
	status = WEXITSTATUS(status);
	debug("\tkid=%d exited with %d\n\n", pid, status);
	assert(status == exit_code);
	exit_code++;

	// should be equiv.
	pid = make_kid(exit_code);
	debug("testing <wait(&status)>\n");
	if(wait(&status) != pid)
		sys_die(wait, returned different pid);
	status = WEXITSTATUS(status);
	debug("\tkid=%d exited with %d\n\n", pid, status);
	assert(status == exit_code);
	exit_code++;

	// async version
	int r, npoll = 0;
	pid = make_kid(exit_code);
	debug("testing <waitpid(-1, &status, WNOHANG)>\n");
	while(1) {
		if((r = waitpid(-1, &status, WNOHANG)) < 0)
			sys_die(waitpid, failed);
		else if(r == 0)
			npoll++;
		else {
			demand(r == pid, what other option?);
			if(r == pid)
				break;
		}
	}
	status = WEXITSTATUS(status);
	// NOTE: on my laptop, npoll is > 3M. 
	debug("\tkid=%d exited with %d, polled %d times.\n\n", 
						pid, status, npoll);
	assert(status == exit_code);
	fprintf(stderr, "SUCCESS\n");

	return 0;
}
