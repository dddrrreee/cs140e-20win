// engler: Trivial variation of program from linux manpage to demonstrate
// select(): echos anything you type into the terminal 3 times (need a carriage
// return). prints a message if nothing has happened for 2sec.
//
// NOTE: select() is O(n) w.r.t. the number of file descriptors.  
// For us, that's fine.  For a server with thousands of connections, 
// it's less so.  Use epoll (if avail) or equiv in such cases.
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "demand.h"

int main(void) {
	/* Watch stdin (fd 0) to see when it has input. */
	fd_set rfds;
	FD_ZERO(&rfds);

	/* Wait up to five seconds. */
	struct timeval tv;
	for(int i = 0; i < 3; i++) {
		// NOTE: *MUST* re-set <tv> and <rfds> after each select() 
		// call.  The specification allows (but does not require) that
		// it modifies them.  As a result, testing what your
		// system actually does to them is no-guarentee of how
		// select() behaves on other systems.
		//
		// setting tv_sec = tv_usec=0 makes select() non-blocking.
		FD_SET(0, &rfds);
		tv.tv_sec = 2;
		tv.tv_usec = 0;

		int retval;
        if((retval = select(1, &rfds, NULL, NULL, &tv)) < 0) {
            sys_die(select, failed?);
		} else if(!retval) {
            printf("No data within two seconds.\n");
		} else {
			printf("Data is available now.\n");
			assert(FD_ISSET(0, &rfds));
			char buf[1024];
			int n;
			if((n = read(0, buf, sizeof buf)) <= 0)
				sys_die(read, failed);
			// we know the last character is '\n'
			buf[n-1] = 0;
			printf("got: <%s>\n", buf);
		} 
	}
	exit(0);
}
