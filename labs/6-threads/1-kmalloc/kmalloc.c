#include "rpi.h"

// symbol created by libpi/memmap, placed at the end
// of all the code/data in a pi binary file.
extern char __heap_start__;

// track if initialized.
static int init_p;

// Module-level variable for the heap
static char* heap_ptr;

#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))

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
    
    void* to_allocate = heap_ptr;
    unsigned rounded_nbytes = roundup(nbytes, 4);
    heap_ptr += rounded_nbytes;
    memset(heap_ptr, 0, rounded_nbytes);
    return to_allocate;

}

// address of returned pointer should be a multiple of
// alignment.
void *kmalloc_aligned(unsigned nbytes, unsigned alignment) {
    demand(init_p, calling before initialized);

    if(alignment <= 4)
        return kmalloc(nbytes);
    demand(alignment % 4 == 0, "weird alignment: not a multiple of 4!");
    unsigned aligned_heap_ptr = (unsigned) heap_ptr;
	aligned_heap_ptr = roundup(aligned_heap_ptr, alignment);
	heap_ptr = (void*) aligned_heap_ptr;

	return kmalloc(nbytes);
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
    heap_ptr = &__heap_start__;
}

/* 
 * free all allocated memory: reset the heap 
 * pointer back to the beginning.
 */
void kfree_all(void) {
    heap_ptr = &__heap_start__;
}

// return pointer to the first free byte.
// for the current implementation: the address <addr> of any
// allocated block satisfies: 
//    assert(<addr> < kmalloc_heap_ptr());
// 
void *kmalloc_heap_ptr(void) {
    return (void*) heap_ptr;
}
