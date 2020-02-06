/*
 * engler, cs140e: simple tests to check that you are handling rising and falling
 * edge interrupts correctly.
 *
 * NOTE: make sure you connect your GPIO 20 to your GPIO 21 (i.e., have it "loopback")
 */
#include "rpi.h"
#include "timer-interrupt.h"
#include "libc/circular.h"
#include "sw-uart.h"
#include "cycle-count.h"

enum { out_pin = 21, in_pin = 20 };
static volatile unsigned n_rising_edge, n_falling_edge;

#define N 128
struct event {
    unsigned v;
    unsigned ncycles;
} events[N];
static volatile unsigned nevents;

// client has to define this.
void interrupt_vector(unsigned pc) {
    // you don't have to check anything else besides
    // if a gpio event was detected: 
    //  - increment n_falling_edge if it was a falling edge
    //  - increment n_rising_edge if it was rising, 
    // make sure you clear the GPIO event!

    unsigned s = cycle_cnt_read();
    dev_barrier();
    unimplemented();
    dev_barrier();
}

void notmain() {
    uart_init();
    cycle_cnt_init();
	
    int_init();

    // use pin 20 for tx, 21 for rx
    sw_uart_t u = sw_uart_init(out_pin,in_pin, 115200);
    gpio_int_rising_edge(in_pin);
    gpio_int_falling_edge(in_pin);

    system_enable_interrupts();

    assert(gpio_read(in_pin) == 1);

    // starter code.
    // make sure this works first, then try to measure the overheads.

    // this will cause transitions every time, so you can compared times.
    unsigned b = 0b01010101;
    sw_uart_putc(&u, b);
    delay_ms(100);
    printk("nevent=%d\n", nevents);
    struct event * e = &events[0];
    for(int i = 0; i < nevents; i++,e++) 
        printk("\tv=%d, cyc=%d\n", e->v, e->ncycles);
    printk("SUCCESS!\n");
    clean_reboot();
}
