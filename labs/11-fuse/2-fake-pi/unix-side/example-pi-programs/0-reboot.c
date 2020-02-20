/* trivial blink */
#include "rpi.h"

void notmain(void) {
    uart_init();
    printk("hello: gonna reboot\n");
    clean_reboot();
}
