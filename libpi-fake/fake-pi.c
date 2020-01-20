// fake implementations of pi-code so that we can check hall in user space.

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "pi-test.h"

void dev_barrier(void) {
    trace("dev barrier\n");
}
void uart_init(void) { 
    trace("uart\n");
}
void rpi_reboot(void) {
    trace("raw reboot\n");
    exit(0);
}
void clean_reboot(void) {
    trace("clean reboot\n");
    exit(0);
}
int printk(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
        printf("PI:");
        int res = vfprintf(stdout, fmt, args);
        fflush(stdout);
    va_end(args);
    return res;
}


/************************************************************************
 * building fake time is kind of not-great.  we just advance the clock
 * by a random amount, capped via time_delta.
 */

// how much time advances on each call.  we assume its about 1 usec per call (this is
// to high, but whatvs)
static int time_usec_inc = 1;

// make sure we get an overflow.
static int time_usec = ~0 - 100;

unsigned timer_get_usec(void) {
    time_usec += time_usec_inc;

    // not sure we should print?
    // trace("returning time=%dusec\n", time_usec++);
    return time_usec;
}

// when it returns, time should move forward <us> usec
void delay_us(unsigned us) {
    trace("delay_us = %dusec\n", us);
    time_usec += us;
}

// when it returns, time should move forward <ms> msec (1ms = 1000usec)
void delay_ms(unsigned ms) {
    trace("delay_ms = %dms\n", ms);
    time_usec += ms * 1000;
}

// in case they didn't include rpi
void (nop)(void) {}

/************************************************************************
 * not using yet
 */

static int interrupts_on = 0;

void int_init(void) {
    interrupts_on = 1;
    trace("int init\n");
}
void system_enable_interrupts(void) {
    trace("system enable interrupts\n");
}
void system_disable_interrupts(void) {
    trace("system disable interrupts\n");
}
