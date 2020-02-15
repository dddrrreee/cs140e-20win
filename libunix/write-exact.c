#include <unistd.h>
#include <assert.h>

#include "libunix.h"

#if 0
int write_exact(int fd, const void *data, unsigned n) {
    unimplemented();
    return n;
}

#else
// not sure if we should allow these to fail or not.
int write_exact_can_fail(int fd, const void *data, unsigned n) {
    assert(n);
    int got;
    if((got = write(fd, data, n)) < 0)
        sys_die(write, write_exact failed);
    if(got != n) {
        debug("expected a write of %d bytes, got %d\n", n, got);
        return 0;
    }
    return n;
}

int write_exact(int fd, const void *data, unsigned n) {
    if(!write_exact_can_fail(fd,data,n))
        panic("write_exact of %d bytes failed\n", n);
    return n;
}

#endif
