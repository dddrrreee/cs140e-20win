#include "rpi.h"

// roughly 2-3x the number of cyles.  dunno if we care.  can read cycle count from
// cp15 iirc.
void delay_cycles(unsigned ticks) {
    while(ticks-- > 0)
        asm("add r1, r1, #0");
}

// in usec
unsigned timer_get_usec(void) {
    return GET32(0x20003004);
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
