// print a single hello world and do a clean reboot. this will only
// work with the bootloader.   clean_reboot will cause my-install to 
// exit.  you should be able to run this over and over from the 
// command line.
#include "rpi.h"

void notmain(void) {
	uart_init();
	printk("hello world\n");
	clean_reboot();
}
