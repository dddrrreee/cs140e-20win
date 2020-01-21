#include "pi-test.h"

/************************************************************************
 * sort-of fake time: we just advance the clock by a <time_usec_inc> at each call.
 */

// how much time advances on each call.  we assume its about 1 usec per call (this is
// to high, but whatvs)
static int time_usec_inc = 1;

// make sure we get an overflow.
static int time_usec = ~0 - 100;

unsigned timer_get_usec(void) {
    time_usec += time_usec_inc;
    trace("getting usec = %dusec\n", time_usec);

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

void fake_time_init(unsigned init_time) {
    time_usec = init_time;
}
