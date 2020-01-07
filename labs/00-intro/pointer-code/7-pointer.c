#include <stdlib.h>

// as far as gcc concerned: opaque functions.
void lock(void), unlock(void);

struct foo { int x; };

/*
 * assume this gets called as helper function for the "double check idiom" discussed
 * in eraser.  e.g.,:
 *   if(!foo)
 *       init_foo(&foo);
 *   ... use foo...
 */
void init_foo(struct foo **out) {
    lock();
    // did someone initialize <out> in the meantime?  if so, just return.
    if(*out) 
        goto done;

    // alloc&initialize the <foo> structure
    struct foo *f = malloc(sizeof *f);
    f->x = 2;

    // assign it to <out>.  this is a "commit point" --- 
    // after this assignment we assume <*out> is defined
    *out = f;  
            
done:
    unlock();
    return;
}
