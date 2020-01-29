/* trivial, sleazy test of kmalloc: don't modify!  */
#include "rpi.h"

void notmain() {
    uart_init();

    printk("starting test\n");
    kmalloc_init();

    printk("heap starts at 0x%x\n", kmalloc_heap_ptr());

    void *heap0 = kmalloc_heap_ptr();
    // sleazy check that assumes how kmalloc works.
    unsigned *start = kmalloc(1);
    printk("start=0x%x\n", start);
    for(unsigned i = 0; i < 20; i++) {
        unsigned *u = kmalloc(1);
        unsigned v = (i << 24 | i << 16 | i << 8 | i);
        *u = v;
    }

    unsigned *u = start+1;
    for(unsigned i = 0; i < 20; i++) {
        unsigned v = (i << 24 | i << 16 | i << 8 | i);
        demand(u[i] == v, 
            "invalid start[%d] =%x, should be %x\n", u[i],v);
    }

    void *p = kmalloc_aligned(1,64);
    demand((unsigned)p % 64 == 0, bug in kmalloc_aligned);

    kfree_all();
    void *heap1 = kmalloc_heap_ptr();
    demand(heap0 == heap1, did not reset the heap correctly!);
    demand(start == kmalloc(1), did not reset the heap correctly!);

    printk("successful test\n");
	clean_reboot();
}
