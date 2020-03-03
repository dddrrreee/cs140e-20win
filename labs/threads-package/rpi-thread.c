// engler, cs140e: starter code for trivial threads package.
#include "rpi.h"
#include "rpi-thread.h"

#define E rpi_preemptive_thread_t
#include "Q.h"

#define PREEMTION_TIME 0x500

// currently only have a single run queue and a free queue.
static Q_t runq, freeq;

static rpi_preemptive_thread_t *cur_thread;        // current running thread.

//potentially get rid of scheduler thread because idk if it's necessary
static rpi_preemptive_thread_t *scheduler_thread;  // first scheduler thread.

// monotonically increasing thread id.
static unsigned tid = 1;
// total number of blocks we have allocated.
static unsigned nalloced = 0;

// return pointer to the current thread.
rpi_preemptive_thread_t *rpi_cur_thread_preemptive(void) {
    return cur_thread;
}

#define is_aligned(_p,_n) (((unsigned)(_p))%(_n) == 0)

// keep a cache of freed thread blocks.  call kmalloc if run out.
static rpi_preemptive_thread_t *th_alloc(void) {
    rpi_preemptive_thread_t *t = Q_pop(&freeq);

    // stack has to be 8-byte aligned.
    if(!t) {
        t = kmalloc_aligned(sizeof *t, 8);
        nalloced++;
    }
    demand(is_aligned(&t->stack[0],8), stack must be 8-byte aligned!);
    t->tid = tid++;
    return t;
}

static void th_free(rpi_preemptive_thread_t *th) {
    // push on the front in case helps with caching.
    Q_push(&freeq, th);
}

/***********************************************************************
 * implement the code below.
 */

/*
 * context switch interrupt
 * decide whether or not to preemt currently running thread and do so if so.
 *
 * MAKE SURE YOU DON'T MESS WITH THE STACK IN THIS FN, OR ELSE IT WILL SCREW WITH THE CSWITCH
 */
void interrupt_vector(unsigned pc) {
    //check to see if was a timer interupt, and clear it if so.
    dev_barrier();
    unsigned pending = GET32(IRQ_basic_pending);
    if((pending & RPI_BASIC_ARM_TIMER_IRQ) == 0)
        return;
    PUT32(arm_timer_IRQClear, 1);
    dev_barrier();

    /* ------perform the context switch------- */
    rpi_preemptive_thread_t * old_thread = cur_thread;
    if(old_thread->done){
        th_free(old_thread);
    }else{
        Q_append(&runq, old_thread);
    }

    if(Q_nelem(&runq) <= 1){
        //there's only one thread left, so we want to resume synchronous
        system_disable_interrupts(); //this isn't working.
    }

    rpi_preemptive_thread_t * new_thread = Q_pop(&runq);

    cur_thread = new_thread;
    rpi_cswitch_preemptive_banked(old_thread->stack, new_thread->stack);
}

// create a new thread.
rpi_preemptive_thread_t *rpi_fork_preemptive(void (*code)(void *arg), void *arg) {
    rpi_preemptive_thread_t *t = th_alloc();

    /*
     * do the brain-surgery on the new thread stack here.
     * must set up initial stack:
     *  - set LR, SP, and store <code> and <arg> where trampoline can get it.
     */
    void rpi_init_trampoline_preemptive(void);
    t->stack[4] = (uint32_t) code;
    t->stack[5] = (uint32_t) arg;
    t->stack[REG_SP_OFF] = (uint32_t) &t->stack[THREAD_MAXSTACK - 2];
    t->stack[REG_PC_OFF] = (uint32_t) &rpi_init_trampoline_preemptive; //set the starting PC. stack[REG_PC_OFF] stores the PC thread was interrupted at / the PC thread should go to when resuming.

    t->done = 0; //mark as not done

    Q_append(&runq, t);
    return t;
}

// exit current thread.
void rpi_exit_preemptive(int exitcode) {
    /*
     * Just mark as done, and the context switch handler will deal with the rest
     * Once marked as done, then spin.
     *
     * Why spin?
     * This thread is done, so just spin. Once the context switch interrupt
     * happens, it will pull the processor off of this spin, and throw away
     * this process, so this infinite loop will be ended, and forgotten about.
     */
    cur_thread->done = 1; //thread is done.
    if(Q_nelem(&runq) <= 1){
        //Should be disabling interrupts here or something
    }
    while(1){ /* spin */ }
}

/*
 * starts the thread system.
 * note: our caller is not a thread!  so you have to figure
 * out how to handle this.
 */
void rpi_thread_start_preemptive(void) {
    // statically check that the stack is 8 byte aligned.
    AssertNow(offsetof(rpi_preemptive_thread_t, stack) % 8 == 0);
    // statically check that the register save area is at offset 0.

    // no other runnable thread: return.
    if(Q_empty(&runq))
        return;

    //  1. create a new fake thread
    //  2. dequeue a thread from the runq
    //  3. context switch to it, saving current state in
    //        <scheduler_thread>
    scheduler_thread = th_alloc();
    cur_thread = scheduler_thread;

    //init interrupts so that we can be preempted
    cycle_cnt_init();
    int_init();
    timer_interrupt_init(PREEMTION_TIME);
    system_enable_interrupts();
}

// check the current thread stack.
void rpi_stack_check_preemptive(void) {
    unimplemented();
}

/********************************************************************
 * save for homework.
 */

// block caller until thread <th> completes.
void rpi_join_preemptive(rpi_preemptive_thread_t *th) {
    if(th == scheduler_thread){
        return; //to prevent deadlock; so that scheduler doesn't wait on itself.
    }
    //continually loop through freeq and runq and check if freed
    while(1){
        unsigned found = 0;
        rpi_preemptive_thread_t * curr = Q_start(&runq);
        //check to see if thread found on runq
        if(th == cur_thread){
            found = 1;
        }else{ //or check to see if it's found on runq
            while(curr != runq.tail){
                if(curr == th){
                    found = 1;
                }
                curr = Q_next(curr);
            }
        }

        //loop through freeq to see if it's been freed
        curr = Q_start(&freeq);
        while(curr != freeq.tail){
            if(curr == th){
                return;
            }
            curr = Q_next(curr);
        }

        if(!found){
            return; //the passed in thread isn't a thread that is running or has finished running i.e. it's garbage
        }
    }
}

// put the current thread on a blocked queue: resume in
// exactly // <usec> micro-seconds.  not before, not after.
// should give low-error resumption times.
void rpi_exact_sleep_preemptive(uint32_t usec) {
    unimplemented();
}
