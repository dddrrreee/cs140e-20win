// engler, cs140e.
//
// driver for bootloader for an r/pi connected via a tty-USB device.
//
// To make grading easier:
//              DO NOT MODIFY THIS CODE!
//              DO NOT MODIFY THIS CODE!
//              DO NOT MODIFY THIS CODE!
//              DO NOT MODIFY THIS CODE!
//              DO NOT MODIFY THIS CODE!
//
// You shouldn't have to modify any code in this file.  Though, if you find
// a bug or improve it, let me know!
//
// Sketch: 
//  1. read in program to send.
//	2. find the USB-tty serial device
//	3. open the USB-tty serial device in 8n1 mode.
//	4. send the program using a custom protocol.
//	5. echo the output from the pi.
//
// For HW1, you will write implement steps (2), (3), (4).
//
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>

#include "../shared-code/simple-boot.h"

#include "libunix.h"

// hack-y state machine to indicate when we've seen the special string
// 'DONE!!!' from the pi telling us to shutdown.
static int done(unsigned char *s) {
	static unsigned pos = 0;
	const char exit_string[] = "DONE!!!\n";
	const int n = sizeof exit_string - 1;

	for(; *s; s++) {
		assert(pos < n);
		if(*s != exit_string[pos++]) {
			pos = 0;
			return done(s+1); // check remainder
		}
		// maybe should check if "DONE!!!" is last thing printed?
		if(pos == sizeof exit_string - 1)
			return 1;
	}
	return 0;
}

// replace any unprintable characters with a space.
// otherwise terminals can go haywire/bizarro.
// note, the string can contain 0's, so we send the
// size.
static void remove_nonprint(uint8_t *buf, int n) {
    for(int i = 0; i < n; i++) {
        uint8_t *p = &buf[i];
        if(isprint(*p) || (isspace(*p) && *p != '\r'))
            continue;
        *p = ' ';
    }
}


// read and echo the characters from the usbtty until it closes 
// (pi rebooted) or we see a string indicating a clean shutdown.
static void echo(int fd, const char *portname) {    
    output("listening on ttyusb=<%s>\n", portname);

    while(1) {
        unsigned char buf[4096];
        int n = read(fd, buf, sizeof buf - 1);

        if(!n) {
            // this isn't the program's fault.  so we exit(0).
            if(tty_gone(portname))
                clean_exit("pi ttyusb connection closed.  cleaning up\n");
            // so we don't keep banginging on the CPU.
            // usleep(1000);
        } else if(n < 0) {
            sys_die(read, "pi connection closed.  cleaning up\n");
        } else {
            buf[n] = 0;
            remove_nonprint(buf,n);
            output("%s", buf);

            if(done(buf)) {
                output("\nSaw done\n");
                clean_exit("\nbootloader: pi exited.  cleaning up\n");
            }
        }
    }
    notreached();
}

// for simplicity support exactly two invocations:
//      my-install /dev/<ttyusb name> <program.name>
//      my-install <program.name>
int main(int argc, char *argv[]) { 
    char *progname = argv[0];

    // open ttyUSB and set to 8n1
    char *dev_name;
    if(argc == 2)
        dev_name = find_ttyusb();
    else if(argc == 3)
        dev_name = argv[1];
    else
        die("%s: wrong number (n=%d) of arguments: expected 1 or 2\n", progname, argc-1);
    int fd = set_tty_to_8n1(open_tty(dev_name), B115200, 1);

    // get program to boot.
    char *pi_prog = argv[argc-1];
	unsigned nbytes;
    uint8_t *code = read_file(&nbytes, pi_prog);

	output("%s: tty-usb=<%s> program=<%s> about to boot\n", progname, dev_name, pi_prog);
    simple_boot(fd, code, nbytes);

    // echo input / output to/from pi
	echo(fd, dev_name);
	return 0;
}
