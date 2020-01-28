#include <assert.h>
#include <unistd.h>

#include "libunix.h"


int read_exact_can_fail(int fd, void *data, unsigned n) {
    assert(n);
    int got;
    if((got = read(fd, data, n)) < 0)
        sys_die(read, read_exact failed);
    if(got != n) {
        debug("expected a read of %d bytes, got %d\n", n, got);
        return 0;
    }
    return n;
}

// if this fails, can be b/c the endpoint died.  we don't propagate this back.
int read_exact(int fd, void *data, unsigned n) {
    if(!read_exact_can_fail(fd,data,n))
        panic("read_exact of %d bytes failed\n", n);
    return n;
}

