#ifndef __SW_UART_H__
#define __SW_UART_H__

// simple structure for sw-uart.  you'll likely have to extend in the future.
//  - rx: the GPIO pin used for receiving.  (make sure you set as input!)
//  - tx: the GPIO pin used for transmitting.  (make sure you set as output!)
//  - the baud rate: initially 115200, identical to what we already use.  if you
//      change this, you will also have to change the baud rate in pi-cat!
//  - cycle_per_bit: the number of cycles we have to hold the TX low (0) or 
//    high (1) to transmit a bit.  or, conversely, the number of cycles the 
//    RX pin will be held low or high by the sender.
typedef struct {
    uint8_t tx,rx;
    uint32_t baud;
    uint32_t cycle_per_bit;  // usec we send each bit.
} sw_uart_t;

// we give you this: printk that uses sw_uart
int sw_uart_printk(sw_uart_t *uart, const char *fmt, ...);
// we give you this: output a null-terminated string.
void sw_uart_putk(sw_uart_t *uart, const char *msg);

// we inline compute usec_per_bit b/c we don't have division on the pi.  division
// by a constant allows the compiler to pre-compute.
#define sw_uart_init(_tx,_rx,_baud) \
    sw_uart_init_helper(_tx,_rx, _baud, (700*1000*1000UL)/_baud)
sw_uart_t sw_uart_init_helper(uint8_t tx, uint8_t rx, uint32_t baud, uint32_t cyc_per_bit);

// blocks until it gets a character.
int sw_uart_getc(sw_uart_t *uart);

// same as <sw_uart_gets_until> (below) but blocking.
int sw_uart_gets_until_blk(sw_uart_t *u, uint8_t *buf, uint32_t nbytes, uint8_t end);

/************************************************************************
 * implement the following.
 */

// output a single character: semantically identical to uart_putc.
void sw_uart_putc(sw_uart_t *uart, unsigned char c);

// returns -1 if no input after <timeout_usec>
int sw_uart_getc_timeout(sw_uart_t *uart, int timeout_usec);

// read from uart <u>, storing the results in <buf> until either
//    1. you read character <end>
//    2. you run out of buffer space.
// returns:
//    number of characters read (including the last one) if no timeout
//    -nbytes if timeout.
// 0-terminates <buf>
int sw_uart_gets_until(sw_uart_t *u, uint8_t *buf, uint32_t nbytes, uint8_t end, uint32_t timeout_usec);

// read from uart <u>, storing the results in <buf> until:
//    1. you have not received any input in <timeout> usec.
//    2. you ran out of buffer space.
// returns number of bytes read (possibly 0) and 0-terminates <buf>
int sw_uart_gets_timeout(sw_uart_t *u, uint8_t *buf, uint32_t nbytes, uint32_t timeout);

#endif
