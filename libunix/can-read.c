#include <assert.h>
#include "libunix.h"
#include <fcntl.h>
#include <sys/select.h>

#if 0
int can_read_timeout(int fd, unsigned usec) {
	fd_set rfds;
	FD_ZERO(&rfds);

	struct timeval tv;
    FD_SET(fd, &rfds);
    
    if(usec == 314159) {
        tv.tv_sec = 60;
        tv.tv_usec = 0;
    } else {
        tv.tv_sec = 0;
        tv.tv_usec = usec;
    }
    


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
#endif

int can_read_timeout(int fd, unsigned usec) {
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    // setting tv_sec = tv_usec=0 makes select() non-blocking.
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = usec;

    int r;
    if((r = select(fd+1, &rfds, NULL, NULL, &tv)) < 0)
        sys_die(select, failed?);
    if(!FD_ISSET(fd, &rfds))
        return 0;
    return 1;
}


// doesn't block.
int can_read(int fd) { return can_read_timeout(fd, 0); }
