// engler, cs140e: create a pseudo-terminal, and try to set its baud rate, etc
// as we do for a real /dev/ttyUSB device and then read/write it.  we need this 
// for lab 5-replay!
// 
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>

#include "demand.h"

#ifdef  __APPLE__
#   include <util.h>
#else
#   include <pty.h>
#endif


int set_tty_to_8n1(int fd, unsigned speed, double timeout) {
    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr (fd, &tty) != 0)
        sys_die(tcgetattr, "tcgetattr failed\n");

    memset (&tty, 0, sizeof tty);

    // https://github.com/rlcheng/raspberry_pi_workshop
    cfsetspeed(&tty, speed);

    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars

    // XXX: wait, does this disable break or ignore-ignore break??
    tty.c_iflag &= ~IGNBRK;         // disable break processing
    tty.c_lflag = 0;                // no signaling chars, no echo,
    // no canonical processing
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN]  = 0;            // read doesn't block
    assert(timeout < 100 && timeout > 0);
    // 0.1 seconds read timeout
    tty.c_cc[VTIME] = (int)timeout *10;            

	/*
	 * Setup TTY for 8n1 mode, used by the pi UART.
	 */

    // Disables the Parity Enable bit(PARENB),So No Parity 
    tty.c_cflag &= ~PARENB; 	
    // CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit 
    tty.c_cflag &= ~CSTOPB;   	
    // Clears the mask for setting the data size     
    tty.c_cflag &= ~CSIZE;	 	
    // Set the data bits = 8
    tty.c_cflag |=  CS8; 		
    // No Hardware flow Control 
    tty.c_cflag &= ~CRTSCTS;
    // Enable receiver,Ignore Modem Control lines 
    tty.c_cflag |= CREAD | CLOCAL; 	
    	
    // Disable XON/XOFF flow control both i/p and o/p
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);    
    // Non Cannonical mode 
    tty.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  
    // No Output Processing
    tty.c_oflag &= ~OPOST;	

    if(tcsetattr (fd, TCSANOW, &tty) != 0)
        sys_die(tcsetattr, "tcsetattr failed\n");
    return fd;
}

// usage: my-install [-silent] [/<dev path>]  progname
int main(int argc, char *argv[]) { 

    int master_fd, slave_fd;
    char name[1024];

    int ret = openpty(&master_fd, &slave_fd, name, 0, 0);

    if(ret < 0)
        sys_die(openpty, "openpty failed!\n");

    debug("output file=<%s>\n", name);

    int fd = open(name, O_RDWR | O_NOCTTY | O_SYNC);
    if(fd < 0)
        sys_die(open, "couldn't open port\n");

    fd = set_tty_to_8n1(fd, B115200, 1);

    unsigned in,out = 0x12345678;
    if(write(fd, &out, 4) != 4)
        sys_die(write, "failed");
    if(read(master_fd, &in, 4) != 4)
        sys_die(write, "failed");
    assert(in == out);
    debug("x=%x\n", in);

    out = 0x87654321;
    if(write(master_fd, &out, 4) != 4)
        sys_die(write, "failed");
    if(read(fd, &in, 4) != 4)
        sys_die(write, "failed");
    assert(in == out);
    debug("x=%x\n", in);


#if 0
    // ok: now we close the master fd and unlink the pty
    // and verify that the client fails on a read.
    output("going to try to unlink <%s>\n", name);
    if(unlink(name) < 0) {
        debug("cant unlink\n");
        sleep(200);
        sys_die(unlink, unlink failed);
    }
#endif

    // this one got stuck before: now it should timeout after about a second
    // with a 0.
    debug("before close: ret=%d\n", (int)read(fd, &in, 1));

    close(master_fd);
    close(slave_fd);
    // this one completes immediately
    ret = read(fd, &in, 1);
    debug("after close, ret=%d\n", ret);

	return 0;
}
