/*
 * first basic test: will succeed even if you have not implemented context switching
 * as long as you:
 *  - simply have rpi_exit(0) and rpi_yield() return rather than context switch to 
 *    another thread.
 *  - [even more basic if you have issues: have rpi_fork simply run immediately]
 * 
 * this test is useful to make sure you have the rough idea of how the threads 
 * should work.  you should rerun when you have your full package working to 
 * ensure you get the same result.
 */

#include "rpi.h"
#include "rpi-thread.h"

static unsigned thread_count, thread_sum;

// trivial first thread: does not block, explicitly calls exit.
static void thread_code(void *arg) {
    unsigned *x = arg;

    // check tid
    unsigned tid = rpi_cur_thread()->tid;
	printk("in thread %p, with %x\n", tid, *x);
    demand(rpi_cur_thread()->tid == *x+1, 
                "expected %d, have %d\n", tid,*x+1);

    // check yield.
    rpi_yield();
	thread_count ++;
    rpi_yield();
	thread_sum += *x;
    rpi_yield();
    // check exit
    rpi_exit(0);
}

void notmain() {
    uart_init();
    kmalloc_init();

    printk("about to test summing of 30 threads\n");

    // change this to increase the number of threads.
	int n = 30;
	thread_sum = thread_count = 0;

    unsigned sum = 0;
	for(int i = 0; i < n; i++)  {
        int *x = kmalloc(sizeof *x);
        sum += *x = i;
		rpi_fork(thread_code, x);
    }
	rpi_thread_start();

	// no more threads: check.
	printk("count = %d, sum=%d\n", thread_count, thread_sum);
	assert(thread_count == n);
	assert(thread_sum == sum);
    printk("SUCCESS!\n");
	clean_reboot();
}
