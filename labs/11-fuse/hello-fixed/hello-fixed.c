#include "rpi.h"

// as you can see, b/c we have no OS protection, we have to change code
// to prevent it from killing the machine.  if we had virtual memory
// and priviledge levels we could protect ourselves any user-level code nonsense.
// we will start doing so next week (wed)
void notmain(void) {
	// NB: we can't do this b/c the shell already initialized and resetting
	// uart may reset connection to Unix.
	// uart_init();

	// if not working, try just printing characters.
#if 0
	uart_putc('h');
	uart_putc('e');
	uart_putc('l');
	uart_putc('l');
	uart_putc('o');

	uart_putc(' ');

	uart_putc('w');
	uart_putc('o');
	uart_putc('r');
	uart_putc('l');
	uart_putc('d');

	uart_putc('\n');
#endif
    // extension: make the code use a system call to do a putchar.
    // right now it's going to blast it to the pi-fuse without 
    // intermediation.
	printk("hello world from address %p\n", (void*)notmain);
	return;

	// NB: this is supposed to be a thread_exit().  calling reboot will
	// kill the pi.
	// clean_reboot();
}
