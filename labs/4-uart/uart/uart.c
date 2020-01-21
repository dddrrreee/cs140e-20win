// implement these three functions.
//
#include "rpi.h"

// note: if you need memory barriers, use <dev_barrier()>
void uart_init(void) {
    PUT32(0x111, 4);
}

int uart_getc(void) {
	return 0;
}
void uart_putc(unsigned c) {
}
