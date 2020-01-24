#ifndef __CYCLE_UTIL_H__
#define __CYCLE_UTIL_H__
// simple, useful utilities for using the pi cycle counterss

#include "cs140e-src/cycle-count.h"

// 700mz machine
#define CYC_PER_SEC (700 * 1000 * 1000UL)
    
// delay <n> cycles assuming we started counting cycles at time
// <start>.  
static inline void delay_ncycles(unsigned start, unsigned n) {
    // this handles wrap-around, but doing so adds two instructions,
    // which makes the delay not as tight.
    while((cycle_cnt_read() - start) < n)
        ;
}   

// write value <v> to GPIO <pin>: return when <ncycles> have passed since
// time <start>
static inline void
write_cyc_until(unsigned pin, unsigned v, unsigned start, unsigned ncycles) {
    // can make a big difference to make an inlined version of GPIO_WRITE_RAW
    // after you do: if you look at the generated assembly, there's some
    // room for tricks.  also, remove the pin check.

    // GPIO_WRITE_RAW(pin,v);
    gpio_write(pin,v);
    delay_ncycles(start,ncycles);
}
    
#endif
