/*
 * write code to allow blinking using arbitrary pins.
 * Implement:
 *    - gpio_set_output(pin) --- set GPIO <pin> as an output (vs input) pin.
 *    - gpio_set_on(pin) --- set the GPIO <pin> on.
 *     - gpio_set_off(pin) --- set the GPIO <pin> off.
 * Use the minimal number of loads and stores to GPIO memory.
 *
 * start.s defines a of helper functions (feel free to look at the assembly!  it's
 *  not tricky):
 *      uint32_t get32(volatile uint32_t *addr)
 *              --- return the 32-bit value held at <addr>.
 *
 *      void put32(volatile uint32_t *addr, uint32_t v)
 *              -- write the 32-bit quantity <v> to <addr>
 *
 * Check-off:
 *  1. get a single LED to blink.
 *  2. attach an LED to pin 19 and another to pin 20 and blink in opposite order (i.e.,
 *     one should be on, while the other is off).   Note, if they behave weirdly, look
 *     carefully at the wording for GPIO set.
 */
#include "gpio.h"

/*
 * These routines are given by us and are in start.s
 */
// writes the 32-bit value <v> to address <addr>:   *(unsigned *)addr = v;
void put32(volatile void *addr, unsigned v);
// returns the 32-bit value at <addr>:  return *(unsigned *)addr
unsigned get32(const volatile void *addr);
// does nothing.
void nop(void);
void dsb(void);
void dmb(void);

// see broadcomm documents for magic addresses.
#define GPIO_BASE 0x20200000
volatile unsigned *gpio_fsel0 = (void*)(GPIO_BASE + 0x00);
volatile unsigned *gpio_set0  = (void*)(GPIO_BASE + 0x1C);
volatile unsigned *gpio_clr0  = (void*)(GPIO_BASE + 0x28);

volatile unsigned *GPFSEL0 = (void*) 0x20200000;
volatile unsigned *GPFSEL1 = (void*) 0x20200004;
volatile unsigned *GPFSEL2 = (void*) 0x20200008;
volatile unsigned *GPFSEL3 = (void*) 0x2020000C;
volatile unsigned *GPFSEL4 = (void*) 0x20200010;
volatile unsigned *GPFSEL5 = (void*) 0x20200014;

volatile unsigned *GPSET0 = (void*) 0x2020001C;
volatile unsigned *GPSET1 = (void*) 0x20200020;

volatile unsigned *GPCLR0 = (void*) 0x20200028;
volatile unsigned *GPCLR1 = (void*) 0x2020002C;

volatile unsigned *GPLEV0 = (void*) 0x20200034;
volatile unsigned *GPLEV1 = (void*) 0x20200038;
// Part 1 implement gpio_set_on, gpio_set_off, gpio_set_output

// set <pin> to be an output pin.  note: fsel0, fsel1, fsel2 are contiguous in memory,
// so you can use array calculations!
void gpio_set_output(unsigned pin) {
    // implement this
    // use gpio_fsel0
    
    if (pin >= 32) {
        return;
    }
    
    volatile unsigned* gpio_reg;
    
    asm volatile ("" : : : "memory");
    //dmb();
    //dsb();
    unsigned reg_select = pin / 10;
    
    // Calculate register bank for selected GPIO pin
    switch (reg_select) {
        case 0: {gpio_reg = GPFSEL0; break;}
        case 1: {gpio_reg = GPFSEL1; break;}
        case 2: {gpio_reg = GPFSEL2; break;}
        case 3: {gpio_reg = GPFSEL3; break;}
        case 4: {gpio_reg = GPFSEL4; break;}
        case 5: {gpio_reg = GPFSEL5; break;}
        default: gpio_reg = 0x0;
    }
    
    // Calculate bitmask for selected GPIO pin
    unsigned bitmask = get32(gpio_reg);
    bitmask &= ~(0b111 << ((pin % 10) * 3));
    bitmask |= (1 << ((pin % 10) * 3));
    put32(gpio_reg, bitmask);
    
    //dmb();
    //dsb();
    asm volatile ("" : : : "memory");
}


// set GPIO <pin> on.
void gpio_set_on(unsigned pin) {
    // implement this
    // use gpio_set0
    if (pin >= 32) {
        return;
    }
    // Get current register value
    volatile unsigned* gpio_reg = 0x0;
    unsigned bitmask = 0x0;
    asm volatile ("" : : : "memory");
    //dmb();
    //dsb();
    
    if (pin <= 31){
        gpio_reg = GPSET0;
        // TODO: why do we not want this statement?
        //bitmask = get32(GPSET0);
    } else {
        gpio_reg = GPSET1;
        //bitmask = get32(GPSET1);
        // TODO: why do we not want this statement?
    }
    
    // Calculate bitmask to preserve values
    bitmask |= 1 << (pin % 32);
    
    put32(gpio_reg, bitmask);
    
    //dmb();
    //dsb();
    asm volatile ("" : : : "memory");
}

// set GPIO <pin> off
void gpio_set_off(unsigned pin) {
    // implement this
    // use gpio_clr0
    if(pin >= 32) {
        return;
    }
    volatile unsigned* gpio_reg = 0x0;
    unsigned bitmask = 0x0;
    asm volatile ("" : : : "memory");
    //dmb();
    //dsb();
    
    if (pin <= 31){
        gpio_reg = GPCLR0;
        //bitmask = get32(GPCLR0);
    } else {
        gpio_reg = GPCLR1;
        //bitmask = get32(GPCLR1);
    }
    
    // Calculate bitmask to preserve values
    bitmask |= 1 << (pin % 32);
    
    put32(gpio_reg, bitmask);
    
    //dmb();
    //dsb();
    asm volatile ("" : : : "memory");
}

// Part 2: implement gpio_set_input and gpio_read

// set <pin> to input.
void gpio_set_input(unsigned pin) {
    // implement.
    if(pin >= 32) {
        return;
    }
    
    volatile unsigned* gpio_reg;
    
    asm volatile ("" : : : "memory");
    //dmb();
    //dsb();
    
    unsigned reg_select = pin / 10;
    
    // Calculate register bank for selected GPIO pin
    switch (reg_select) {
        case 0: {gpio_reg = GPFSEL0; break;}
        case 1: {gpio_reg = GPFSEL1; break;}
        case 2: {gpio_reg = GPFSEL2; break;}
        case 3: {gpio_reg = GPFSEL3; break;}
        case 4: {gpio_reg = GPFSEL4; break;}
        case 5: {gpio_reg = GPFSEL5; break;}
        default: gpio_reg = 0x0;
    }
    
    // Calculate bitmask for selected GPIO pin
    unsigned bitmask = get32(gpio_reg);
    bitmask &= ~(0b111 << ((pin % 10) * 3));
    put32(gpio_reg, bitmask);
    
    //dmb();
    //dsb();
    asm volatile("" : : : "memory");
    
}

// return the value of <pin>
int gpio_read(unsigned pin) {
    
    if(pin >= 32) {
        return -1;
    }
    
    unsigned v = 0;
    
    asm volatile ("" : : : "memory");
    //dmb();
    //dsb();
    
    if (pin <= 31){
        v = get32(GPLEV0);
    } else {
        v = get32(GPLEV1);
    }
    
    //dmb();
    //dsb();
    asm volatile ("" : : : "memory");
    
    return (v & (1 << (pin % 32)));
}

// set <pin> to <v> (v \in {0,1})
void gpio_write(unsigned pin, unsigned v) {
    
    if(pin >= 32) {
        return;
    }
    
    if(v)
        gpio_set_on(pin);
    else
        gpio_set_off(pin);
}

