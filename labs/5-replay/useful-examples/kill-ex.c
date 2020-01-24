// engler, cs140e: show how to catch control-c and handle writes to a closed pipe.
// usage: run and control-c it.
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

#include "demand.h"

static int pid;

// if(signal(SIGPIPE, catch_sigpipe) < 0)
void catch_siguser(int sig) {
    debug("pid=%d, caught a siguser!\n", getpid());
    exit(0);
}

static volatile int caught_sigint = 0;
void handler_sigint(int sig) {
    debug("caught a sigint!\n");
    caught_sigint = 1;
}

void catch_cntrl_c(void (*handler)(int)) {
    // at this point need to catch control-c, otherwise will loose the log record.
    struct sigaction sig;
    sig.sa_handler = handler;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = 0;
    if(sigaction(SIGINT, &sig, NULL) < 0)
        sys_die(sigaction, could not install);
}


int main(void) {
    // try removing this: what happens?
    if(signal(SIGPIPE, SIG_IGN) < 0)
        sys_die(signal, cannot install sigpipe);

    if(signal(SIGUSR1, catch_siguser) < 0)
        sys_die(signal, cannot install siguser);

	if(!(pid=fork())) {
        // if you get rid of this both processes will get a control-c and the 
        // the child will die.
        if(setsid() < 0)
            sys_die(setsid, setsid-failed);
        // child: busy wait until killed.
        while(1)
            ;
	}

    catch_cntrl_c(handler_sigint);
    // you should control-c this process.
    while(!caught_sigint)
        ;
	debug("done waiting: about to kill child=%d\n", pid);
    if(kill(pid, SIGUSR1) < 0)
        sys_die(kill, kill failed?);

    int status;
    if(waitpid(pid, &status, 0) < 0)
        sys_die("waitpid", waitpid failed?);
    if(WIFSIGNALED(status))
        panic("kid killed by signal: %d (sig=%d)\n", status, WTERMSIG(status));

    if(!WIFEXITED(status))
        panic("status failed: status=%d, exitstatus=%d\n", status, WEXITSTATUS(status));
    debug("status=%d\n", WEXITSTATUS(status));
	return 0;
}
