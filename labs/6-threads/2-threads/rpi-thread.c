// engler, cs140e: starter code for trivial threads package.
#include "rpi.h"
#include "rpi-thread.h"

// typedef rpi_thread_t E;
#define E rpi_thread_t
#include "Q.h"

static Q_t runq, freeq;
static unsigned nthreads;

static rpi_thread_t *cur_thread;        // current running thread.
static rpi_thread_t *scheduler_thread;  // first scheduler thread.

// return pointer to the current thread.  
rpi_thread_t *rpi_cur_thread(void) {
    return cur_thread;
}

// keep a cache of freed thread blocks.  call kmalloc if run out.
static rpi_thread_t *th_alloc(void) {
    rpi_thread_t *t = Q_pop(&freeq);
    if(!t)
        t = kmalloc(sizeof *t);
    return t;
}

/***********************************************************************
 * implement the code below.
 */

// create a new thread.
rpi_thread_t *rpi_fork(void (*code)(void *arg), void *arg) {
    rpi_thread_t *t = th_alloc();

    // do the brain-surgery on the new thread stack here.
    unimplemented();

    Q_append(&runq, t);
    return t;
}

// exit current thread.
void rpi_exit(int exitcode) {
	/*
	 * 1. free current thread.
	 *
	 * 2. if there are more threads, dequeue one and context
 	 * switch to it.
	 
	 * 3. otherwise we are done, switch to the scheduler thread 
	 * so we call back into the client code.
	 */
	unimplemented();
}

// yield the current thread.
void rpi_yield(void) {
	// if cannot dequeue a thread from runq
	//	- there are no more runnable threads, return.  
	// otherwise: 
	//	1. put current thread on runq.
	// 	2. context switch to the new thread.
	unimplemented();
}

/*
 * starts the thread system.  
 * note: our caller is not a thread!  so you have to figure
 * out how to handle this.
 */
void rpi_thread_start(void) {
    // statically check that the stack is 8 byte aligned.
    AssertNow(offsetof(rpi_thread_t, stack) % 8 == 0);

    // no other runnable thread: return.
    if(Q_empty(&runq))
        return;

    //  1. create a new fake thread 
    //  2. dequeue a thread from the runq
    //  3. context switch to it, saving current state in
    //	    <scheduler_thread>
    scheduler_thread = th_alloc();
    unimplemented();
    printk("THREAD: done with all threads, returning\n");
}

/************************************************************
 * you can save the following for the homework assignment.
 */

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
