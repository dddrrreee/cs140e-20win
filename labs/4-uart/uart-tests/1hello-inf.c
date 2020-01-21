// print an infinite number of hello world's to the terminal.  this allows
// you to attach to your terminal with pi-cat to see the output.  (otherwise
// the output will be gone by the time you run pi-cat).
#include "rpi.h"

void notmain(void) {
	uart_init();
	while(1)
		printk("hello world\n");
}
