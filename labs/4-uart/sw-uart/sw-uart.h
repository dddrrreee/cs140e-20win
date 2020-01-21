#ifndef __SW_UART_H__
#define __SW_UART_H__

typedef struct {
    int tx,rx;
    unsigned baud;
    unsigned usec_per_bit;  // usec we send each bit.
} sw_uart_t;


// we inline compute usec_per_bit b/c we don't have division on the pi.  division
// by a constant allows the compiler to pre-compute.
#define sw_uart_init(_tx,_rx,_baud) \
    (sw_uart_t){ .tx = _tx, .rx = _rx, .baud = _baud, .usec_per_bit = (1000*1000)/_baud }

void sw_uart_putc(sw_uart_t *uart, unsigned char c);
void sw_uart_putk(sw_uart_t *uart, const char *msg);

// returns -1 if no input after <timeout_usec>
int sw_uart_getc(sw_uart_t *uart, int timeout_usec);

#endif
