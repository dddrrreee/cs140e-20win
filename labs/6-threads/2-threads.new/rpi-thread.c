// engler, cs140e: starter code for trivial threads package.
#include "rpi.h"
#include "rpi-thread.h"

// typedef rpi_thread_t E;
#define E rpi_thread_t
#include "Q.h"

// currently only have a single run queue and a free queue.
static Q_t runq, freeq;

static rpi_thread_t *cur_thread;        // current running thread.
static rpi_thread_t *scheduler_thread;  // first scheduler thread.

// monotonically increasing thread id.
static unsigned tid = 1;
// total number of blocks we have allocated.
static unsigned nalloced = 0;

// return pointer to the current thread.  
rpi_thread_t *rpi_cur_thread(void) {
    return cur_thread;
}

#define is_aligned(_p,_n) (((unsigned)(_p))%(_n) == 0)

// keep a cache of freed thread blocks.  call kmalloc if run out.
static rpi_thread_t *th_alloc(void) {
    rpi_thread_t *t = Q_pop(&freeq);

    // stack has to be 8-byte aligned.
    if(!t) {
        t = kmalloc_aligned(sizeof *t, 8);
        nalloced++;
    }
    demand(is_aligned(&t->stack[0],8), stack must be 8-byte aligned!);
    t->tid = tid++;
    return t;
}

static void th_free(rpi_thread_t *th) {
    // push on the front in case helps with caching.
    Q_push(&freeq, th);
}

/***********************************************************************
 * implement the code below.
 */

// create a new thread.
rpi_thread_t *rpi_fork(void (*code)(void *arg), void *arg) {
    rpi_thread_t *t = th_alloc();

    /*
     * do the brain-surgery on the new thread stack here.
     * must set up initial stack: 
     *  - set LR, SP, and store <code> and <arg> where trampoline can get it.
     */
    void rpi_init_trampoline(void);

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
    // statically check that the register save area is at offset 0.
    AssertNow(offsetof(rpi_thread_t, reg_save_area) == 0);

    // no other runnable thread: return.
    if(Q_empty(&runq))
        return;
    rpi_internal_check();

    unimplemented();
    return;

    //  1. create a new fake thread 
    //  2. dequeue a thread from the runq
    //  3. context switch to it, saving current state in
    //	    <scheduler_thread>
    scheduler_thread = th_alloc();
    unimplemented();
    printk("rpithreads: done with all threads! returning\n");
}


// internal consistency checks.
void rpi_internal_check(void) {
    // the blocks on the runq + freeq must equal the total ever allocated
    // or we have leaked storage.
    unsigned n_free = Q_nelem(&freeq),
             n_run = Q_nelem(&runq),
             n = n_free + n_run;

    if(nalloced != n)
        panic("storage leak: should have %d free blocks, have %d (runq=%d, freeq=%d)\n", 
            nalloced, n, n_free,n_run);

    // active thread system: 
    //      walk through the run queue making sure the stack pointer
    //     and ra pointer makes sense.
    if(!Q_empty(&runq)) {
        unimplemented();
        return;
    }
    printk("thread: internal check passed\n");
}

// check the current thread stack.
void rpi_stack_check(void) {
    unimplemented();
}

/********************************************************************
 * save for homework.
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
