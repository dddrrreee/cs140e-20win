#ifndef __UART_INT_H__
#define __UART_INT_H__
#include "cs140e-src/uart.h"

typedef enum { UART_NO_INT = 0, UART_TX_INT = 1111, UART_RX_INT = 2222 } uart_int_t;

typedef struct aux_periphs hw_uart_t;

// has to be called last!   make sure you don't do a subsequent uart_init()
void uart_init_with_interrupts(void);

uart_int_t uart_has_int(hw_uart_t *uart);
void uart_clear_int(hw_uart_t *uart);

unsigned *const uart_get_aux_irq(void) ;

int uart_getc_int(void);
// int uart_putc_int(uint8_t c);
int uart_interrupt_handler(void);

extern volatile unsigned n_uart_interrupts;

int uart_has_data_int(void);

#endif
