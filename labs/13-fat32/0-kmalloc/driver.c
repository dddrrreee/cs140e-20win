/*
 * engler, cs140e: simple test of kmalloc.
 */
#include "rpi.h"
#include "libc/helper-macros.h"

#define trace(args...) do { printk("TRACE:"); printk(args); } while(0)

int memzeroed(const void *_p, unsigned n) {
    const char *p = _p;
    for(int i = 0; i < n; i++)
        if(p[i] != 0)
            return 0;
    return 1;
}

static void do_allocs(void) {
    trace("going to check alignment\n");
    for(int i = 1; i < 20; i++) {
        void *p = kmalloc(i);
        trace("kmalloc(%d)= 0x%x\n", i, p);

        // check alignment.
        assert(is_aligned((unsigned)p,8));
        // check that is zero filled
        assert(memzeroed(p, i));
    }

    trace("larger allocations\n");
    for(int i = 32; i < 1024 * 1024; i *= 2) {
        unsigned n = rpi_rand16() % 1024*16;
        void *p = kmalloc_aligned(n,i);
        trace("kmalloc_aligned(1,%d) =0x%x\n", i, p);

        // check alignment.
        assert(is_aligned((unsigned)p,i));
        // check that is zero filled
        assert(memzeroed(p, n));
    }
}

void notmain(void) {
    uart_init();

    trace("TRACE: setting malloc start to 1mb\n");
    kmalloc_init_set_start(1024 * 1024);

    void *p = kmalloc(1);
    assert(is_aligned((unsigned)p, 1024 * 1024));
    do_allocs();

    clean_reboot();
}
