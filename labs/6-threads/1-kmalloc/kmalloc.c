#include "rpi.h"

// symbol created by libpi/memmap, placed at the end
// of all the code/data in a pi binary file.
extern char __heap__start__;

// track if initialized.
static int init_p;

/*
 * Return a memory block of at least size <nbytes>
 * Notes:
 *  - There is no free, so is trivial: should be just 
 *    a few lines of code.
 *  - The returned pointer should always be 4-byte aligned.  
 *    Easiest way is to make sure the heap pointer starts 4-byte
 *    and you always round up the number of bytes.  Make sure
 *    you put an assertion in.  
 */
void *kmalloc(unsigned nbytes) {
    demand(init_p, calling before initialized);
    unimplemented();
}

/*
 * One-time initialization, called before kmalloc 
 * to setup heap. 
 *    - should be just a few lines of code.
 *    - sets heap pointer to the location of 
 *      __heap_start__.   print this to make sure
 *      it makes sense!
 */
void kmalloc_init(void) {
    demand(!init_p, cannot initialize twice!\n);
    init_p = 1;
    unimplemented();
}

/* 
 * free all allocated memory: reset the heap 
 * pointer back to the beginning.
 */
void kfree_all(void) {
    unimplemented();
}

// return pointer to the first free byte.
// for the current implementation: the address <addr> of any
// allocated block satisfies: 
//    assert(<addr> < kmalloc_heap_ptr());
// 
void *kmalloc_heap_ptr(void) {
    unimplemented();
}
