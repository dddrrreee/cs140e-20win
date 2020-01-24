// engler, cs140e: showing how to check if an fd is open
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

static int is_open(int fd) {
	return fcntl(fd, F_GETFL) >= 0;
}

int main(void) { 
    // 0 = stdin, 1 = stdout, 2 = stderr.  these should be open.
    //  already setup when this process runs.
	assert(is_open(0));
	assert(is_open(1));
	assert(is_open(2));
    // we never opened 3, so it should be closed.
	assert(!is_open(3));
	printf("SUCCESS!\n");
	return 0;
}

