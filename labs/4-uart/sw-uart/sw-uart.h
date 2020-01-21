#ifndef __SW_UART_H__
#define __SW_UART_H__

// simple structure for sw-uart.  you'll likely have to extend in the future.
//  - rx: the GPIO pin used for receiving.  (make sure you set as input!)
//  - tx: the GPIO pin used for transmitting.  (make sure you set as output!)
//  - the baud rate: initially 115200, identical to what we already use.  if you
//      change this, you will also have to change the baud rate in pi-cat!
//  - usec_per_bit: the number of micro-seconds (1000*1000 in 1 second) that we
//      have to hold the TX low (0) or high (1) to transmit a bit.  or, conversely,
//      the number of usec the RX pin will be held low or high by the sender.
typedef struct {
    int tx,rx;
    unsigned baud;
    unsigned usec_per_bit;  // usec we send each bit.
} sw_uart_t;

// we inline compute usec_per_bit b/c we don't have division on the pi.  division
// by a constant allows the compiler to pre-compute.
#define sw_uart_init(_tx,_rx,_baud) \
    (sw_uart_t){ .tx = _tx, .rx = _rx, .baud = _baud, .usec_per_bit = (1000*1000)/_baud }

// output a single character: semantically identical to uart_putc.
void sw_uart_putc(sw_uart_t *uart, unsigned char c);
// output a null-terminated string.
void sw_uart_putk(sw_uart_t *uart, const char *msg);

// returns -1 if no input after <timeout_usec>
int sw_uart_getc(sw_uart_t *uart, int timeout_usec);

#endif
