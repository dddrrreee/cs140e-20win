/*
 * simple example of how to do a system call
 */
#include "rpi.h"
#include "rpi-interrupts.h"

int syscall_hello(const char *msg);
int syscall_illegal(void);

// client has to define this.
void interrupt_vector(unsigned pc) {
    panic("should not be called\n");
}

// pc should point to the system call instruction.
//      can see the encoding on a3-29:  lower 24 bits hold the encoding.
// r0 = the first argument passed to the system call.
int syscall_vector(unsigned pc, uint32_t r0) {
    uint32_t inst, sys_num;

    // figure out the instruction and the system call number.
    unimplemented();

    switch(sys_num) {
    case 1: 
            printk("syscall: <%s>\n", (const char *)r0); 
            return 0;
    default: 
            printk("illegal system call = %d!\n", sys_num);
            return -1;
    }
}

void notmain() {
    uart_init();
	
    printk("about to install handlers\n");
    int_init();

    printk("about to run syscall hello\n");
    int res = syscall_hello("hello world");
    printk("result of calling system call 1=%d\n", res);
    assert(res == 0);

    printk("about to call illegal system call: should die\n");
    res = syscall_illegal();
    printk("result of illegal system call =%d\n", res);
    assert(res == -1);

    clean_reboot();
}
