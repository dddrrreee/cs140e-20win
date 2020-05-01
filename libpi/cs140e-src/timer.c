#include "rpi.h"

// roughly 2-3x the number of cyles.  dunno if we care.  can read cycle count from
// cp15 iirc.
void delay_cycles(unsigned ticks) {
#ifndef RPI_UNIX
    while(ticks-- > 0)
        asm("add r1, r1, #0");
#else
    void dummy_fn(void);
    while(ticks-- > 0)
        dummy_fn();
#endif
}

// no dev barrier.
unsigned timer_get_usec_raw(void) {
    return GET32(0x20003004);
}

// in usec
unsigned timer_get_usec(void) {
// comment these out for this lab (8) so student
// code does not get hit by weird timing issues.
//    dev_barrier();
    unsigned u = timer_get_usec_raw();
//    dev_barrier();
    return u;
}

void delay_us(unsigned us) {
    unsigned rb = timer_get_usec();
    while (1) {
        unsigned ra = timer_get_usec();
        if ((ra - rb) >= us) {
            break;
        }
    }
}
void delay_ms(unsigned ms) {
    delay_us(ms*1000);
}
void delay_sec(unsigned sec) {
    delay_ms(sec*1000);
}
