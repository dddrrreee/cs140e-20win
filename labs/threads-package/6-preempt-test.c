/*
 * Self-made test for simple preemptive thread checking 
 */

#include "rpi.h"
#include "rpi-thread.h"

#define NUM_THREADS 5

static volatile int i;
static volatile rpi_thread_t* thread_list [NUM_THREADS];

void print_one(void* arg) {
	int a = 0;
	printk("Thread 1 says %d\n", (unsigned)arg);
	a = 123;
	printk("Spinning to wait for preemption\n");
	for(long i = 0; i < 10000000; i++) {
		;
	}
	printk("a is %d\n", a);
	a = 987;
	printk("Second spin\n");
	for(long i = 0; i < 10000000; i++) {
		;
	}
	printk("a is %d\n", a);
}

void print_two(void* arg) {
	printk("Thread 2 says %d\n", (unsigned)arg);
}

void print_three(void* arg) {
	printk("Thread 3 says %d\n", (unsigned)arg);
}

void print_four(void* arg) {
	printk("Thread 4 says %d\n", (unsigned)arg);
}

void print_five(void* arg) {
	printk("Thread 5 says %d\n", (unsigned)arg);
}


void notmain() {
	uart_init();
	kmalloc_init();

	i = 1;
	rpi_fork(print_one, (void*)i);
	i++;
	rpi_fork(print_two, (void*)i);
	i++;
	rpi_fork(print_three, (void*)i);
	i++;
	rpi_fork(print_four, (void*)i);
	i++;
	rpi_fork(print_five, (void*)i);

	printk("Starting now!\n");
	rpi_thread_start();
	printk("SUCCESS!\n");
	clean_reboot();
}
