#include <sys/types.h>
#include <sys/wait.h>
#include "libunix.h"

// asynchronously check if <pid> has exited: 
// returns:
//    - 0 if not.
//    - 1 if so, with raw exit status in <status>
int child_exit_noblk(int pid, int *status) {
    int r;

    if((r = waitpid(pid, status, WNOHANG)) < 0)
        sys_die(waitpid, failed);
    if(!r)
        return 0;
    demand(r == pid, invalid child);
    return 1;
}

// non-blocking check if <pid> exited cleanly.
// returns:
//   - 0 if not exited;
//   - 1 if exited cleanly (exitcode in <status>, 
//   - -1 if exited with a crash (status holds reason)
int child_clean_exit_noblk(int pid, int *status) {
    if(!child_exit_noblk(pid, status))
        return 0;
    // crash.
    if(!WIFEXITED(*status))
        return -1;
    *status = WEXITSTATUS(*status);
    return 1;
}

/*
 * blocking check that child <pid> exited cleanly.
 * returns:
 *  - 1 if exited cleanly, exitcode in <status>
 *  - 0 if crashed, reason in <status> .
 */
int child_clean_exit(int pid, int *status) {
    int ret;

    if((ret = waitpid(pid, status, 0)) < 0)
        sys_die(waitpid, fatal);
    if(ret != pid)
        panic("impossible: waitpid returned when <%d> wasn't dead\n", pid);

    // crashed.
    if(!WIFEXITED(*status))
        return 0;
    // clean.
    *status = WEXITSTATUS(*status);
    return 1;
}

