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

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#include "libunix.h"
#include "simple-boot.h"


#define TRACE_FD 21
int is_fd_open(int fd) {
    return fcntl(fd, F_GETFL) >= 0;
}
void handoff_to(int fd, char *argv[]) {
    // don't dup if we are already running with <TRACE_FD> open.
    if(fd != TRACE_FD)
        dup2(fd, TRACE_FD);

    int pid = fork();
    if(pid < 0)
        sys_die("fork", cannot fork process?);
    if(pid == 0) {
        argv_print("about to handoff to", argv);
        if(execvp(argv[0], argv) < 0)
            sys_die(execvp, bogus);
    }
    int status;
    if(!child_clean_exit(pid, &status))
        die("child: <%s> crashed\n", argv[0]);
    output("my-install: <%s> exited with: %d\n", argv[0], status);
}

// for simplicity support exactly two invocations:
//      my-install /dev/<ttyusb name> <program.name>
//      my-install <program.name>
int main(int argc, char *argv[]) { 
    char *progname = argv[0];

    char **exec_argv = 0;

    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-exec") == 0) {
            argv[i++] = 0;
            if(i == argc) 
                die("%s: bad use of -exec: need a program name\n", progname);
            argc = i-1;
            exec_argv = &argv[i];
            output("exec: <%s>\n", exec_argv[0]);
        }
    }
    char *dev_name;
    if(argc == 2)
        dev_name = find_ttyusb_first();
    else if(argc == 3)
        dev_name = argv[1];
    else
        die("%s: wrong number (n=%d) of arguments: expected 1 or 2\n", progname, argc-1);

    int fd = TRACE_FD;
    if(!is_fd_open(fd)) 
        fd = set_tty_to_8n1(open_tty(dev_name), B115200, 1);
    else
        output("doing a fake trace!\n");

    // get program to boot.
    char *pi_prog = argv[argc-1];
	unsigned nbytes;
    uint8_t *code = read_file(&nbytes, pi_prog);

	output("%s: tty-usb=<%s> program=<%s> about to boot\n", progname, dev_name, pi_prog);
    simple_boot(fd, code, nbytes);

    if(!exec_argv)
        pi_echo(0, fd, dev_name);
    else 
        handoff_to(fd, exec_argv);
	return 0;
}
