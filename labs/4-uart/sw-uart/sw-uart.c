#include "rpi.h"
#include "sw-uart.h"

// helper: cleans up the code.
static inline void timed_write(int pin, int v, unsigned usec) {
    gpio_write(pin,v);
    delay_us(usec);
}

// do this first: used timed_write to cleanup.
//  recall: time to write each bit (0 or 1) is in <uart->usec_per_bit>
void sw_uart_putc(sw_uart_t *uart, unsigned char c) {
    unimplemented();
}

// do this second: you can type in pi-cat to send stuff.
//      EASY BUG: if you are reading input, but you do not get here in 
//      time it will disappear.
int sw_uart_getc(sw_uart_t *uart, int timeout_usec) {
    unimplemented();
}

void sw_uart_putk(sw_uart_t *uart, const char *msg) {
    for(; *msg; msg++)
        sw_uart_putc(uart, *msg);
}

