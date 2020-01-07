#include <stdlib.h>

// as far as gcc concerned: opaque functions.
void lock(void), unlock(void);

struct foo { int x; };

void double_check_lock_idiom(struct foo **out) {
    if(*out)
        return;

    lock();
    if(!*out) {
        struct foo *f = malloc(sizeof *f);
        f->x = 2;
        // what happens if we remove?
        asm volatile ("" : : : "memory");
        *out = f;
    }
    unlock();
    return;
}
