//  PLEASE DON"T MODIFY THIS FILE YET --- I'm going to add some comments, etc.
//  PLEASE DON"T MODIFY THIS FILE YET --- I'm going to add some comments, etc.
//  PLEASE DON"T MODIFY THIS FILE YET --- I'm going to add some comments, etc.
//  PLEASE DON"T MODIFY THIS FILE YET --- I'm going to add some comments, etc.
//
// implement:
//  uart_init
//
//  int uart_can_getc(void);
//  int uart_getc(void);
//
//  int uart_can_putc(void);
//  void uart_putc(unsigned c);
//
// see that hello world works.
//
//
#include "rpi.h"

// called first to setup uart to 8n1 115200  baud,
// no interrupts.
//  - you will need memory barriers, use <dev_barrier()>
//
//  later: should add an init that takes a baud rate.
void uart_init(void) {
}

// 1 = at least one byte on rx queue, 0 otherwise
static int uart_can_getc(void) {
    return 0;
}

// returns one byte from the rx queue, if needed
// blocks until there is one.
int uart_getc(void) {
	return 0;
}

// 1 = space to put at least one byte, 0 otherwise.
int uart_can_putc(void) {
    return 0;
}

// put one byte on the tx qqueue, if needed, blocks
// until TX has space.
void uart_putc(unsigned c) {
}


// simple wrapper routines useful later.

// a maybe-more intuitive name for clients.
int uart_has_data(void) {
    return uart_can_getc();
}

int uart_getc_async(void) { 
    if(!uart_has_data())
        return -1;
    return uart_getc();
}
