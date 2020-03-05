#include "rpi.h"
#include "rpi-thread.h"

/************************************************************************
 * you can save the following for the homework assignment.
 */

// block caller until thread <th> completes.
void rpi_join(rpi_thread_t *th) {
    unimplemented();
}

// put the current thread on a blocked queue: resume in 
// exactly // <usec> micro-seconds.  not before, not after.
// should give low-error resumption times.
void rpi_exact_sleep(uint32_t usec) {
    unimplemented();
}

// fork a thread that guarantees it will 
// only run for <usec> until blocking.  this allows
// us to check for bad behavior / scheduling conflicts.
rpi_thread_t *
rpi_fork_bounded(void (*code)(void *arg), void *arg) {
    unimplemented();
}

struct rpi_thread_stats {
    unsigned hit_deadlines,  // number of sleeps we were off on.
                missed_deadlines,  // number of sleeps we were off on.
                missed_error,      // total usec of missed deadlines
                created_threads,
                exited_threads;
};
struct rpi_thread_stats rpi_get_stats(void) {
    unimplemented();
}

