/* trivial tests for threads package.  */
#include "rpi.h"
#include "rpi-thread.h"

static unsigned thread_count, thread_sum;

// trivial first thread: does not block, explicitly calls exit.
static void thread_code(void *arg) {
    unsigned *x = arg;
	printk("in thread %p, with %x\n", rpi_cur_thread()->tid, *x);
	thread_count ++;
	thread_sum += *x;
    rpi_exit(0);
}

void notmain() {
    uart_init();
    kmalloc_init();

    printk("about to test summing of 30 threads\n");
	int n = 30;
	thread_sum = thread_count = 0;

	for(int i = 0; i < n; i++)  {
        int *x = kmalloc(sizeof *x);
        *x = i;
		rpi_fork(thread_code, x);
    }
	rpi_thread_start();

	// no more threads: check.
	printk("count = %d, sum=%d\n", thread_count, thread_sum);
	assert(thread_count == n);
	clean_reboot();
}
