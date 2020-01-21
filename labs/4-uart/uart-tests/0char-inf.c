// print infinite "h\n"'s to the terminal.  the simplest program for
// testing your uart code.
#include "rpi.h"

void notmain(void) {
	uart_init();
	while(1) {
		uart_putc('h');
		uart_putc('\n');
	}
}
