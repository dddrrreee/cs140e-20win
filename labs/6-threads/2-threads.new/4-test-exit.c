#include "rpi.h"
#include "rpi-thread.h"


// if our threads are fast enough you can run a bunch of these
// and not see weird flickers.
void trivial(unsigned a0, unsigned stored_sp, unsigned sp) {
    printk("trivial thread: code=%x, stored sp=%x sp=%x\n", a0,stored_sp,sp);
    clean_reboot();
}

void notmain(void) {
    uart_init();
    kmalloc_init();

    rpi_fork(trivial, 0);
    rpi_thread_start();
    printk("SUCCESS\n");
    clean_reboot();
}

void print_and_die(void) { panic("should not call\n"); }
