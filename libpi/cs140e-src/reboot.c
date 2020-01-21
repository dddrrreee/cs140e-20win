// engler, cs140e: reboot the pi.
//
// XXX: need to double check that this handles being called in an exception correctly.
#include "rpi.h"

/*
 * Super nasty error: if you call reboot (or panic) in an exception, it 
 * will just lock up (or call reset).
 * 
 * So, if we get here, drop privileges, and do the reboot.  Not sure if this
 * works if we have a trashed stack pointer?
 */
static unsigned user_level = 0b10000;
int at_user_level(void) {
    unsigned cpsr = 0;
    asm volatile("mrs %0,cpsr" : "=r"(cpsr));

    // user level = lower 5 bits = 0b10000);
    return (cpsr  & 0b11111)  == user_level;
}

// this is just used by the reboot code.
void set_user_level(void) {
    unsigned cpsr = 0;
    // XXX: shouldn't we just set the one bit?
    asm volatile("mrs %0,cpsr" : "=r"(cpsr));
        cpsr = (cpsr & ~0b11111) | user_level;
    asm volatile("msr cpsr, %0" :: "r"(cpsr));
}

void rpi_reboot(void) {
    if(at_user_level()) {
        set_user_level();
        // if you do this, can't have a minimal pi binary: pulls in all sorts
        // of stuff.
        // assert(!at_user_level());
    }

    // gives uart time to flush: should just call flush directly.
    delay_ms(30);

    const int PM_RSTC = 0x2010001c;
    const int PM_WDOG = 0x20100024;
    const int PM_PASSWORD = 0x5a000000;
    const int PM_RSTC_WRCFG_FULL_RESET = 0x00000020;

    // timeout = 1/16th of a second? (whatever)
    PUT32(PM_WDOG, PM_PASSWORD | 1);
    PUT32(PM_RSTC, PM_PASSWORD | PM_RSTC_WRCFG_FULL_RESET);
    while(1); 
}
