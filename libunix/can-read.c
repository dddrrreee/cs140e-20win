#include <assert.h>
#include "libunix.h"
#include <fcntl.h>
#include <sys/select.h>

int can_read_timeout(int fd, unsigned usec) {
	fd_set rfds;
	FD_ZERO(&rfds);

	struct timeval tv;
	
	FD_SET(fd, &rfds);
	tv.tv_sec = 0;
	tv.tv_usec = usec;

	int retval;
	if((retval = select(fd + 1, &rfds, NULL, NULL, &tv)) < 0) {
		//output("Can Read Retval: %d\n", retval);
		sys_die(select, failed);
	} else if (!retval) {
		//output("Can Read Retval: %d\n", retval);
		return 0;
	} else {
		//output("Can Read Retval: %d\n", retval);
		return 1;
	}
}

// doesn't block.
int can_read(int fd) { return can_read_timeout(fd, 0); }
