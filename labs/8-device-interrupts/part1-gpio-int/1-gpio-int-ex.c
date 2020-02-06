/*
 * engler, cs140e: simple tests to check that you are handling rising and falling
 * edge interrupts correctly.
 *
 * NOTE: make sure you connect your GPIO 20 to your GPIO 21 (i.e., have it "loopback")
 */
#include "rpi.h"
#include "timer-interrupt.h"
#include "libc/circular.h"

static volatile int ninterrupt;

enum { out_pin = 21, in_pin = 20 };
static volatile unsigned n_rising_edge, n_falling_edge, n_interrupt;

// client has to define this.
void interrupt_vector(unsigned pc) {
    n_interrupt++;

    // you don't have to check anything else besides
    // if a gpio event was detected: 
    //  - increment n_falling_edge if it was a falling edge
    //  - increment n_rising_edge if it was rising, 
    // make sure you clear the GPIO event!
    unimplemented();
}

static void test1_simple_rise(unsigned t) {
    printk("--------------------------------------------------\n");
    assert(!n_rising_edge);

    printk("should see rising edge!\n");
    gpio_write(out_pin, 1);
    delay_us(t);

    assert(n_rising_edge == 1);

    // leave in initial state
    n_rising_edge = 0;
    printk("TEST1: Passed\n");
}

static void test2_simple_fall(unsigned t) {
    printk("--------------------------------------------------\n");
    assert(!n_falling_edge);

    printk("should see falling  edge!\n");
    gpio_write(out_pin, 0);
    delay_us(t);

    assert(n_falling_edge == 1);

    // leave in initial state
    n_falling_edge = 0;
    printk("TEST2: Passed\n");
}

// test both rising and falling edges: should get rid of delays
// and wait until there is a change.
static void test3_full_test(unsigned t) {
    printk("--------------------------------------------------\n");
    int i;
    unsigned n = 10;

    assert(!n_falling_edge);
    assert(!n_rising_edge);
    for(i = 0; i < n; i++) {
        unsigned b;

        // catch rising edge.
        b = n_rising_edge;
        printk("should see rising edge!  before=%d\n", b);
        gpio_write(out_pin, 1);
        delay_us(t);
        printk("rising after = %d!\n", n_rising_edge);
        assert(b+1 == n_rising_edge);

        // catch falling edge.
        printk("iter=%d\n", i);
        b = n_falling_edge;
        printk("should see a falling edge!  before=%d\n", b);
        gpio_write(out_pin, 0);
        delay_us(t);
        printk("falling after = %d!\n", n_falling_edge);
        assert(b+1 == n_falling_edge);

    }
    printk("done: n=%d, rising=%d, falling=%d\n", n, n_rising_edge, n_falling_edge);
    assert(n_rising_edge == n);
    assert(n_falling_edge == n);
}

void notmain() {
    uart_init();
	
    int_init();

    gpio_set_output(out_pin);

    gpio_set_input(in_pin);
    gpio_int_rising_edge(in_pin);
    gpio_int_falling_edge(in_pin);

    system_enable_interrupts();

    // starter code.
    // make sure this works first, then try to measure the overheads.
    unsigned t = 1000 * 500;
    test1_simple_rise(t);
    test2_simple_fall(t);
    test3_full_test(t);

    printk("SUCCESS!\n");
    clean_reboot();
}
