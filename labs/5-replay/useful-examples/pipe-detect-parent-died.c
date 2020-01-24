// engler: yet another pipe variant: detect that a parent died by waiting for an
// EOF on pipe read.
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "demand.h"

void parent(int fd, int pid) {
	int ret;
	char c;

	// Block til child dies, print exit code.
	int status;
	if(waitpid(pid, &status, 0) < 0)
		sys_die(waitpid, should never fail);
    if(!WIFEXITED(status))
        printf("Child crashed.\n");
    else
        printf("Child exited with status = %d\n", WEXITSTATUS(status));

	// echo whever child sent.
	printf("Parent (pid=%d): about to read dead child output: <", getpid());
	while((ret = read(fd, &c, 1)) == 1)
		putchar(c);
	printf(">\n");

	// read() of closed pipe = EOF, which means ret == 0.  ret < 0
	// is some other error.
	if(ret < 0)
		sys_die(read, returned an error);

	assert(ret == 0);
	printf("SUCCESS: Parent done.\n");
}

void child(int fd) {
	char buf[1024];
	sprintf(buf, "Hello, child.getpid=<%d>", getpid());
	if(write(fd, buf, strlen(buf)+1) < 0)
		sys_die(write, child write failed);
	exit(10);
}

int main(void) {
    int parent_pid = getpid();

	int fds[2];
	if(pipe(fds) < 0)
		sys_die(pipe, cannot create pipe fds);

	int pid;
	if((pid = fork()) < 0)
		sys_die(fork, fork failed);
    
	if(pid) {
        output("parent=%d, going to sleep\n", parent_pid);
        // parent: sleep long enough that we see exit-detection.
        sleep(1);
        output("parent: exiting\n");
    } else {
		if(close(fds[1]) < 0)
			sys_die(close, impossible);

        unsigned char c;
        int res;
        if((res = read(fds[0], &c, 1)) < 0)
            sys_die(read, impossible);
        if(res)
            panic("impossible: got nbytes=%d, back\n", res);

        // unix hack: we can't waitpid for parent, but we can check if it's
        // alive by sending a kill(0) --- result should be an error if dead.
        //
        // NOTE: there is a race condition --- the pid could have been reused.
        output("sending a kill to verify that parent %d died\n", pid);

        // returns 0 on success.
        if(!kill(parent_pid, 0)) {
            panic("kill only succeeds if parent still alive: impossible!\n");
        // -1 on error.
        } else {
            // if we got a permission error, something is weird.
            demand(errno != EPERM, permission error);
            // the is the only acceptable <errno>
            demand(errno == ESRCH, failed for wrong reason);
            output("kill succeeded!\n");
        }
	}
	return 0;
}
