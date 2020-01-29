#include <assert.h>
#include <unistd.h>

#include "libunix.h"

int write_exact_can_fail(int fd, const void *data, unsigned n) {
	assert(n);
	int n_out;
	if((n_out = write(fd, data, n)) < 0) {
		panic("WTH\n");
	}
    return n;
}

int write_exact(int fd, const void* data, unsigned n) {
	if(!write_exact_can_fail(fd, data, n)) {
		panic("Write\n");
	}
	return n;
}

