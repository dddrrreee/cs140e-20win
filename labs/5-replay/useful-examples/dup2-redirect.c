// engler, cs140e: override stdin, stdout, stderr in a program we fork/exec.
// call with the name of a program to run, e.g.,:
//  dup2-redirect ls
//  dup2-redirect ./test-redirect
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "demand.h"

// recall: pipe[0] = read in, pipe[1] = write end.
int redir(int *rd_fd, int *wr_fd, char * const pi_process) {
    int in[2], out[2];

    if(pipe(in) < 0)
        sys_die(pipe, cannot create pipe fds);
    if(pipe(out) < 0)
        sys_die(pipe, cannot create pipe fds);

    int pid;
    if((pid = fork()) < 0)
        sys_die(fork, cannot fail);

    if(!pid) {
        if(dup2(in[0], 0) < 0 
        || dup2(out[1], 1) < 0
        || dup2(out[1], 2) < 0) 
            sys_die(dup, bad dup);

        if(close(in[0]) < 0 
        || close(in[1]) < 0 
        || close(out[0]) < 0 
        || close(out[1]) < 0)
            sys_die(close, bad fd);

        char * const argv[2] = { pi_process, 0 };
        if(execvp(pi_process, argv) < 0)
            sys_die(exec, bad exec call);
    }
    *rd_fd = out[0];
    *wr_fd = in[1];
    if(close(out[1]) < 0 || close(in[0]) < 0)
        sys_die(close, bad fds);
    return 0;
}
// run pi-shell in sub-process with pipes replaced for stdin/stdout.
int main(int argc, char *argv[]) {
    printf("about to try: <%s>\n", argv[1]);
    int ret, rd_fd, wr_fd;
    redir(&rd_fd, &wr_fd, argv[1]);

    // write a character to the subprocess stdin.
    char c = 'h';
    if(write(wr_fd, &c, 1) != 1)
        sys_die(write, write failed);

    // echo its stdout/stderr (both go into the pipe)
    while((ret = read(rd_fd, &c, 1)) == 1)
        putchar(c);
    if(ret < 0)
        sys_die(read, problem with child?);
    printf("done\n");
    return 0;
}
