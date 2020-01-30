// engler, cs140e: starter code for trivial threads package.
#include "rpi.h"
#include "rpi-thread.h"

// typedef rpi_thread_t E;
#define E rpi_thread_t
#include "Q.h"

// currently only have a single run queue and a free queue.
static Q_t runq;

static rpi_thread_t *cur_thread;        // current running thread.
static rpi_thread_t *scheduler_thread;  // first scheduler thread.

// return pointer to the current thread.  
rpi_thread_t *rpi_cur_thread(void) {
    return cur_thread;
}

// keep a cache of freed thread blocks.  call kmalloc if run out.
static rpi_thread_t *th_alloc(void) {
    static Q_t freeq;
    rpi_thread_t *t = Q_pop(&freeq);
    if(!t)
        // stack has to be 8-byte aligned.
        t = kmalloc_aligned(sizeof *t, 8);
    return t;
}

/***********************************************************************
 * implement the code below.
 */

// create a new thread.
rpi_thread_t *rpi_fork(void (*code)(void *arg), void *arg) {
    rpi_thread_t *t = th_alloc();

    // do the brain-surgery on the new thread stack here.
	asm volatile (
		"mov r3, %[thread] \n\t"
		"ldr r2, [r3] \n\t"
		"stmfd r2!, {r0-r15} \n\t"
		"str r0, [r2] \n\t"
		: // output
		:[thread] "r"(&(t->stack))
		 // input
	); 
	

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
	 memset(cur_thread, 0, sizeof(cur_thread));
	 if(!Q_empty(&runq)) {
		 rpi_cswitch(cur_thread->reg_save_area, Q_pop(&runq)->reg_save_area);
	 } else {
		rpi_cswitch(cur_thread->reg_save_area, scheduler_thread->reg_save_area);
	 }
}

// yield the current thread.
void rpi_yield(void) {
	// if cannot dequeue a thread from runq
	//	- there are no more runnable threads, return.  
	// otherwise: 
	//	1. put current thread on runq.
	// 	2. context switch to the new thread.
	rpi_thread_t* next_thread;
	if(!(next_thread = Q_pop(&runq))) {
		return;
	} else {
		Q_append(&runq, cur_thread);
		rpi_cswitch(cur_thread->reg_save_area, next_thread->reg_save_area); 
	}
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

    //  1. create a new fake thread 
    //  2. dequeue a thread from the runq
    //  3. context switch to it, saving current state in
    //	    <scheduler_thread>
    scheduler_thread = th_alloc();
    rpi_thread_t* next_thread = th_alloc();
	next_thread = Q_pop(&runq);
	rpi_cswitch(scheduler_thread->reg_save_area, next_thread->reg_save_area);
    printk("rpithreads: done with all threads! returning\n");
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
