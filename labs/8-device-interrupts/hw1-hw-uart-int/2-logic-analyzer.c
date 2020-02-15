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
#include "cycle-util.h"

void sw_uart_putc(sw_uart_t *uart, unsigned char c) {
    // use local variables to minimize any loads or stores
    int tx = uart->tx;
    uint32_t n = uart->cycle_per_bit,
             u = n,
             s = cycle_cnt_read();
    
    // get start conditions
    unsigned end = n;
    unsigned start = cycle_cnt_read();

    // lower line (line idling)
    write_cyc_until(tx, 0, start, end);
    end += n;
    write_cyc_until(tx, c & 1, start, end);
    end += n;
    write_cyc_until(tx, c & 2, start, end);
    end += n;
    write_cyc_until(tx, c & 4, start, end);
    end += n;
    write_cyc_until(tx, c & 8, start, end);
    end += n;
    write_cyc_until(tx, c & 16, start, end);
    end += n;
    write_cyc_until(tx, c & 32, start, end);
    end += n;
    write_cyc_until(tx, c & 64, start, end);
    end += n;
    write_cyc_until(tx, c & 128, start, end);
    end += n;
    write_cyc_until(tx, 1, start, end);
}

// usec: does not have to be that accurate since the time is just for timeout.
static inline int wait_until_usec(int rx, int v, unsigned timeout_usec) {
    unsigned start = timer_get_usec_raw();
    while(1) {
        if(gpio_read(rx) == v)
            return 1;
        if(timer_get_usec_raw() - start > timeout_usec)
            return 0;
    }
}

// do this second: you can type in pi-cat to send stuff.
//      EASY BUG: if you are reading input, but you do not get here in
//      time it will disappear.
int sw_uart_getc_timeout(sw_uart_t *uart, int timeout_usec) {
   int rx = uart->rx;

    // get start bit: timeout_usec=0 implies you return right away.
    while(!wait_until_usec(rx, 0, timeout_usec))
        return -1;

    // do this first so we have a tighter bound (maybe)
    unsigned s = cycle_cnt_read();  // subtract off slop?

    // store these in locals to minimize load stores later.
    uint32_t u = uart->cycle_per_bit;
    unsigned n = u/2;
    unsigned c = 0;

    // wait one period + 1/2 to get in the middle of the next read.
    delay_ncycles(s, n + 1*u);
    
    s += u;
    c |= (gpio_read(rx) >> rx);
    delay_ncycles(s, u);
    s += u;
    c |= (gpio_read(rx) >> rx) << 1;
    delay_ncycles(s, u);
    s += u;
    c |= (gpio_read(rx) >> rx) << 2;
    delay_ncycles(s, u);
    s += u;
    c |= (gpio_read(rx) >> rx) << 3;
    delay_ncycles(s, u);
    s += u;
    c |= (gpio_read(rx) >> rx) << 4;
    delay_ncycles(s, u);
    s += u;
    c |= (gpio_read(rx) >> rx) << 5;
    delay_ncycles(s, u);
    s += u;
    c |= (gpio_read(rx) >> rx) << 6;
    delay_ncycles(s, u);
    s += u;
    c |= (gpio_read(rx) >> rx) << 7;
    //delay_ncycles(s, u);
    //s += u;
    
    // make sure you wait for a stop bit: otherwise the next read may fail.
    while(!wait_until_usec(rx, 0, timeout_usec))
        return -1;
    // delay_ncycles(s, n);

    return (int)c;
}


// read characters using <sw_uart_getc()> until:
//       <sw_uart_getc() == <end>>
// returns -nbytes read if timeout (could be 0).
int sw_uart_gets_until(sw_uart_t *u, uint8_t *buf, uint32_t nbytes, uint8_t end, uint32_t usec_timeout) {
    assert(nbytes>0);
    buf[0] = 0;

    int i;
    uint8_t char_in;
    for(i = 0; i < nbytes-1; i++) {
        if((char_in = (uint8_t)sw_uart_getc_timeout(u, usec_timeout)) != end) {
            buf[i] = char_in;
        } else {
            break;
        }
    }
    buf[i] = 0;
    return i;
}

// read characters using <sw_uart_getc()> until:
//      we do not receive any characters for <timeout> usec.
// make sure to 0 terminate!
int sw_uart_gets_timeout(sw_uart_t *u, uint8_t *buf,
                    uint32_t nbytes, uint32_t usec_timeout) {

    assert(nbytes>0);
    buf[0] = 0;

    int i;
    for(i = 0; i < nbytes-1; i++) {
        buf[i] = (uint8_t) sw_uart_getc_timeout(u, usec_timeout);
    }
    buf[i] = 0;
    return i;
}

/**************************************************************************
 * this code is implemented for you
 */

sw_uart_t
sw_uart_init_helper(uint8_t tx, uint8_t rx, uint32_t baud, uint32_t cyc_per_bit) {
    // maybe enable the cache?  don't think will work otherwise.
	gpio_set_output(tx);
    gpio_set_input(rx);
	gpio_write(tx, 1);
    cycle_cnt_init();


    // make sure it makes sense.
    unsigned mhz = 700 * 1000 * 1000;
    unsigned derived = cyc_per_bit * baud;
    assert((mhz - baud) <= derived && derived <= (mhz + baud));
    // panic("cyc_per_bit = %d * baud = %d\n", cyc_per_bit, cyc_per_bit * baud);

    return (sw_uart_t) {
            .tx = tx,
            .rx = rx,
            .baud = baud,
            .cycle_per_bit = cyc_per_bit
    };
}

// blocking read.
int sw_uart_getc(sw_uart_t *uart) {
    int res = sw_uart_getc_timeout(uart, ~0);
    if(res < 0)
        panic("impossible: have an infinite timeout!\n");
    return res;
}

void sw_uart_putk(sw_uart_t *uart, const char *msg) {
    for(; *msg; msg++)
        sw_uart_putc(uart, *msg);
}

// don't pollute the rest of the code with all the stuff in the
// <stdarg.h> header.
#include <stdarg.h>
#include "libc/va-printk.h"
int sw_uart_printk(sw_uart_t *uart, const char *fmt, ...) {
    char buf[460];

    va_list args;
    va_start(args, fmt);
        int sz = va_printk(buf, sizeof buf, fmt, args);
    va_end(args);
    assert(sz < sizeof buf-1);
    sw_uart_putk(uart,buf);
    return sz;
}

int sw_uart_gets_until_blk(sw_uart_t *u, uint8_t *buf, uint32_t nbytes, uint8_t end) {
    int res = sw_uart_gets_until(u, buf, nbytes, end, ~0);
    if(res < 0)
        panic("impossible: have an infinite timeout!\n");
    return res;
}


enum { out_pin = 21, in_pin = 20 };
static volatile unsigned n_rising_edge, n_falling_edge;

#define N 128
struct event {
    unsigned v;
    unsigned ncycles;
} events[N];
static volatile unsigned nevents = 0;
static volatile cycle_counter = 0;
// client has to define this.
void interrupt_vector(unsigned pc) {

    // you don't have to check anything else besides
    // if a gpio event was detected:
    //  - increment n_falling_edge if it was a falling edge
    //  - increment n_rising_edge if it was rising,
    // make sure you clear the GPIO event!
    unsigned s = cycle_cnt_read();
    dev_barrier();
    if(is_gpio_int(GPIO_INT0) || is_gpio_int(GPIO_INT1)) {
        if(gpio_read(in_pin) == 0) {
            n_falling_edge++;
			events[nevents].v = 0;
			events[nevents].ncycles = cycle_counter - s;
			nevents++;
        } else if (gpio_read(in_pin) > 0) {
            n_rising_edge++;
			events[nevents].v = 1;
			events[nevents].ncycles = cycle_counter - s;
			nevents++;
        }
        else {
            ;
        }
    }
	cycle_counter = s;
    gpio_event_clear(in_pin);
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

    assert(gpio_read(in_pin)>>in_pin == 1);

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
