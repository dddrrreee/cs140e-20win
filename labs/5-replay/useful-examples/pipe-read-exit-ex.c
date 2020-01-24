// engler: Same as pipe-ex.c except that we read from the child's pipe *after* 
// it exits to show the data is preserved.
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
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
	int fds[2];
	if(pipe(fds) < 0)
		sys_die(pipe, cannot create pipe fds);

	int pid;
	if((pid = fork()) < 0)
		sys_die(fork, fork failed);

	if(pid != 0) {
		if(close(fds[1]) < 0)
			sys_die(close, impossible);
		parent(fds[0], pid);
	} else {
		if(close(fds[0]) < 0)
			sys_die(close, close failed?);
		child(fds[1]);
	} 
	return 0;
}
