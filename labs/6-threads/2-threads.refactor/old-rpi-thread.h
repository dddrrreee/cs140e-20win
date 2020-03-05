// engler,cs140e: trivial non-pre-emptive threads package.
#ifndef __RPI_THREAD_H__

/*
 * trivial thread descriptor:
 *   - <next>: pointer to the next thread in the queue that 
 *     holds the thread.
 *  - <tid> unique thread id.
 *  - <stack>: fixed size stack.  
 *
 * the big simplication: rather than save registers on the 
 * stack we stick them in a single fixed-size location at
 * offset in the thread structure.  
 *  - offset 0 means it's hard to mess up offset calcs.
 *  - doing in one place rather than making space on the 
 *    stack makes it much easier to think about.
 *  - the downside is that you can't do nested, pre-emptive
 *    context switches.  since this is not pre-emptive, we
 *    defer until later.
 *
 * changes:
 *  - dynamically sized stack.
 *  - save registers on stack.
 *  - add condition variables or watch.
 *  - some notion of real-time.
 *  - a private thread heap.
 *  - add error checking: thread runs too long, blows out its 
 *    stack.  
 */

// you should define these; also rename to something better.
#define REG_SP_OFF 10
#define REG_LR_OFF 9

// in bytes.
#define THREAD_MAXSTACK (1024 * 8/4)
typedef struct rpi_thread {
    // as our initial 
    uint32_t reg_save_area[16];

	struct rpi_thread *next;
	uint32_t tid;
    // not needed, but useful for testing without cswitch
    void (*fn)(void *arg);
    void *arg;

	uint32_t stack[THREAD_MAXSTACK];
} rpi_thread_t;

// create a new thread that takes a single argument.
rpi_thread_t *rpi_fork(void (*code)(void *arg), void *arg);

// exit current thread.
void rpi_exit(int exitcode);

// yield the current thread.
void rpi_yield(void);

// starts the thread system: nothing runs before.
// 	- <preemptive_p> = 1 implies pre-emptive multi-tasking.
void rpi_thread_start(void);

// pointer to the current thread.  
rpi_thread_t *rpi_cur_thread(void);

// context-switch: 
//  - save the current register values into <old_save_area>
//  - load the values in <new_save_area> into the registers
//  reutrn to the caller (which will now be different!)
void rpi_cswitch(uint32_t *old_save_area, uint32_t *new_save_area);

// returns the stack pointer --- don't write to 
// what it points to unless you are looking for excitement.
const uint8_t *rpi_get_sp(void);

// check that the current thread is within its stack.
void rpi_stack_check(void);

// sleep for <usec>.  can be used to implement simple
// real-time scheduling.
void rpi_exact_sleep(uint32_t usec);

// do some internal consistency checks --- used for testing.
void rpi_internal_check(void);



#endif
