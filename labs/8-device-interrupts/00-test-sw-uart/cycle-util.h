#ifndef __CYCLE_UTIL_H__
#define __CYCLE_UTIL_H__
#include "rpi.h"
#include "cs140e-src/cycle-count.h"

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
//  
// can make a big difference to make an inlined version of GPIO_WRITE_RAW
// after you do: if you look at the generated assembly, there's some
// room for tricks.  also, remove the pin check.
static inline void
write_cyc_until(unsigned pin, unsigned v, unsigned start, unsigned ncycles) {
    // GPIO_WRITE_RAW(pin,v);
    gpio_write(pin,v);
    delay_ncycles(start,ncycles);
}   

// we can't do this when faking things out, so redefine it.
// #ifdef RPI_UNIX
#if 1
#   define GPIO_READ_RAW gpio_read
#else
    // trying to eek out what we can in terms of speed so that we
    // do not get 
    static inline unsigned GPIO_READ_RAW(unsigned pin) {
        unsigned GPIO_BASE  = 0x20200000UL;
    
        volatile unsigned *gpio_lev0  = (void*)(GPIO_BASE + 0x34);
        unsigned off = (pin%32);
        // this can sign extend?
        return (*gpio_lev0 >> off) & 1;
    }
#endif


// wait until <pin>=<v> or until we spin for <ncycles>
static inline int 
wait_until_cyc(unsigned pin, unsigned v, unsigned s, unsigned ncycles) {
    while(1) {
        // use GPIO_READ_RAW
        if(gpio_read(pin) == v)
            return 1;
        if((cycle_cnt_read() - s) >= ncycles)
            return 0;
    }
}
#endif
