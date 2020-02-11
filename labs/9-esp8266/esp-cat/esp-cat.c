// engler, cs140e simple program to allow you to interactively play 
// with a esp8266.
//
// usage: 
//      esp-cat 
// or
//      esp-cat <log-file>
//
// e.g.:
//      ./esp-cat ./log-file.txt
//
// note: pi-cat would work, but we want to allow script files and also 
// prevent commands from being issued too fast to the esp)
//
// XXX: 
//  - right now the timeout is hardcoded.  makes sense to mess with it.
//  - code duplication.   trying to keep kinda obvious.
//
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>

#include "libunix.h"

static int read_timeout(int fd, void *data, unsigned n, unsigned timeout) {
    if(!can_read_timeout(fd, timeout))
        return 0;
    if((n = read(fd, data, n)) < 0)
        sys_die(read, read failed);
    return n;
}

static int read_esp(int fd, unsigned timeout) {
    // we only want to talk to the esp once per sec so don't overrun.
    char buf[8192];
    int n = read_timeout(fd, buf, sizeof buf, timeout);

    if(!n)
        return 0;
    buf[n] = 0;
    output("%s", buf);
    return 1;
}

static int read_user(int input_fd, int esp_fd) {
    char buf[8192];
    int n = read_timeout(input_fd, buf, sizeof buf, 0);
    if(!n)
        return 0;

    assert(buf[n-1] == '\n');
    buf[n-1] = '\r';
    buf[n]   = '\n';
    buf[n+1]   = 0;
    if(write(esp_fd, buf, n+1) != (n+1))
        sys_die(write, failed?);
    
    // after writing to the esp, make sure we don't hit it 
    // too frequently.
    while(read_esp(esp_fd, 1000*100))
        ;

    return 1;
}

static void put_byte(int fd, uint8_t b) {
    if(write(fd, &b, 1) != 1)
        panic("what is going on?\n");
}

static void run_file(int fd, uint8_t *p) {
    for(; *p; p++) {
        if(*p != '\n') 
            put_byte(fd, *p);
        else {
            put_byte(fd, '\r');
            put_byte(fd, '\n');
            while(read_esp(fd, 1000*100))
                ;
        }
    }
}

int main(int argc, char *argv[]) { 
	const char *dev_name = find_ttyusb();
    output("opening: <%s>\n", dev_name);
    int baud = B115200;
    int fd = set_tty_to_8n1(open_tty(dev_name), baud, 1);

    if(argc == 2) {
        output("opening <%s>\n", argv[1]);
        unsigned size;
        run_file(fd, read_file(&size, argv[1]));
    } else if(argc > 2)
        panic("bad number of arguments.\n");

    while(1) { 
        read_esp(fd, 1000 * 10);
        read_user(0, fd);
    }
	return 0;
}
