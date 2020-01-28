#include <sys/types.h>
#include <signal.h>
#include <errno.h>

#include "libunix.h"

int is_pid_dead(pid_t pid) {
    // returns 0 if the pid is alive and we could send a kill.
    if(!kill(pid, 0))
        return 0;
    demand(errno != EPERM, permission error);
    demand(errno == ESRCH, failed for wrong reason);
    debug("pid <%d> is dead!\n", pid);
    return 1;
}
