// the unix side of the fake virtual machine we are making.  it
// implements a trivial fake-pi runtime system so that we can link
// pi programs against it (on your laptop) and run them.
// 
//  by default it only provides enough to link against 2-blink.c using
//  your GPIO
//  - put32
//  - get32
//  - printk
//  - uart_init
//  - clean_reboot
//  - delay_ms/delay_us
//
// you should extend with
//  - gpio_set_input
//  - gpio_set_output
//  - gpio_read
//  - gpio_write
//
// it's easy to add more stuff, though you might want to restructure so 
// you could just provide put32/get32 and link most everything else from
// the pi.
//
// NOTE: we should really use threads to pull stuff from the pi end, however
// to keep it simple we use the same hack as in your bootloader where we grab
// messages when we grab opcodes from the pi-stream.  you *MUST* use get_op
// on opcode bytes (vs get_byte) for this to work.  it's simpler, but the down
// side is that the output can get delayed significantly and reordered w.r.t.
// what the unix side is printing.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

#include "rpi.h"
#include "libunix.h"
#include "pi-message.h"

/*************************************************************
 * trivial starter code for input/output.  you are welcome do to your
 * own.
 */
enum { pi_fd = HANDOFF_FD };

static void put_uint8(uint8_t b)   { write_exact(pi_fd, &b, 1); }
static void put_uint32(uint32_t u) { write_exact(pi_fd, &u, 4); }

static uint8_t get_uint8(void) { 
    uint8_t b;
    read_exact(pi_fd, &b, 1); 
    return b;
}
static uint32_t get_uint32(void) { 
    uint32_t u;
    read_exact(pi_fd, &u, 4); 
    return u;
}

// *ALWAYS* call <get_op> to parse an op: checks if it is a PI_PRINT_CHAR 
// and emits if so, otherwise returns.   
//
// you could use an extension like this to handle all
// opcodes that can be spontaneously sent by the pi code.
//
// or, if you use threads have a thread waiting on the pi fd and pushing
// output to a circular buf for the consumer thread.
uint8_t get_op(void) {
    uint8_t op;

    while(1) {
        op = get_uint8();
        if(op != PI_PRINT_CHAR)
            return op;
        output("%c", get_uint8());
    }
}

static void expect_op(uint8_t expect) {
    uint8_t got = get_op();
    if(got == expect)
        return;
    panic("expected %d, received %d\n", expect, got);
}

/*************************************************************
 * trivial fake pi stuff: this mirrors what is in libpi-fake,
 * but stripped down so it's easier to get the entire idea.
 */

#if 0
void gpio_set_output(unsigned pin) {
    unimplemented();
}
void gpio_set_input(unsigned pin) {
    unimplemented();
}
void gpio_write(unsigned pin, unsigned val) {
    unimplemented();
}
void gpio_set_on(unsigned pin) {
    unimplemented();
}
void gpio_set_off(unsigned pin) {
    unimplemented();
}

int gpio_read(unsigned pin) {
    gpio_write(pin,0);
}
#endif

void put32(volatile void *addr, unsigned val) {
    unsigned long x64 = (unsigned long) addr;
    uint32_t x32 = x64;
    assert(x32 == x64);
    output("unix: put32(%x : %p) = %x\n", x32, addr, val);
    put_uint8(PI_PUT32);
    put_uint32((unsigned long)addr);
    put_uint32(val);
}

unsigned get32(const volatile void *addr) {
    unsigned long x64 = (unsigned long) addr;
    uint32_t x32 = x64;
    assert(x32 == x64);

    put_uint8(PI_GET32);
    put_uint32((unsigned long)addr);

    output("unix: get32(%x : %p)\n", x32, addr);

    expect_op(PI_GET32_REPLY);
    uint32_t v = get_uint32();
    output("unix: reply: val=%x\n", v);
    return v;
}

void uart_init(void) { }

// just delay using your laptop methods.
void delay_us(unsigned usec)  { usleep(usec); }
void delay_ms(unsigned ms)    { delay_us(ms*1000); }

// fake version of printk: just call printf.
int printk(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
        printf("from local PI print:");
        int res = vfprintf(stdout, fmt, args);
    va_end(args);
    return res;
}
int va_printk(char *buf, unsigned n, const char *fmt, va_list args) {
    return vsnprintf(buf, n, fmt, args);
}

// should have a way to flush all output from the pi.
// right now we just delay, but this is not perfect
// and, also, gross.  viva la cs140e!
void clean_reboot(void) {
    output("about to exit\n");
    // we don't have to wait?
    put_uint8(PI_REBOOT);

    delay_ms(1000);
    while(can_read_timeout(pi_fd,10000))
        get_op();
    exit(0);
}

int main(void) {
    if(!fd_is_open(HANDOFF_FD))
        panic("pi-echo: must run with <HANDOFF_FD> as an open file descriptor\n");
    // our attempt to try to cause stdout to flush right away.
    fflush(stdout);
    setvbuf(stdout, NULL, _IONBF, 0);

    // ping the pi til it echos something back: otherwise we could
    // start shoving stuff into its UART before its booted.
    while(1) {
        put_uint8(PI_READY);
        if(can_read_timeout(pi_fd, 10000)) {
            expect_op(PI_READY);
            break;
        }
    }

    output("about to call not main\n");
    notmain();
    output("not main returned\n");
    clean_reboot();
}
