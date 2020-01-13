// engler, cs140e.
// 
//          DO NOT MODIFY!!!
//          DO NOT MODIFY!!!
//          DO NOT MODIFY!!!
//          DO NOT MODIFY!!!
//          DO NOT MODIFY!!!
//
#ifndef __LIB_UNIX_H__
#define __LIB_UNIX_H__
#include <stdint.h>
#include <sys/types.h>

// bunch of useful macros for debugging/error checking.
#include "demand.h"

// looks in /dev for a ttyusb device. 
// returns:
//  - device name.
// panic's if 0 or more than 1.
char *find_ttyusb(void);

// read in file <name>
// returns:
//  - pointer to the code.  pad code with 0s up to the
//    next multiple of 4.  
//  - bytes of code in <size>
//
// fatal error open/read of <name> fails.
uint8_t *read_file(unsigned *size, const char *name);

//
// we give you the following.  
//

// opens the ttyusb <device> and returns file descriptor.
int open_tty(const char *device);

// used to set a tty to the 8n1 protocol needed by the tty-serial.
int set_tty_to_8n1(int fd, unsigned speed, double timeout);

// returns 1 if the tty is not there, 0 otherwise.
// used to detect when the user pulls the tty-serial device out.
int tty_gone(const char *ttyname);

// roundup <x> to a multiple of <n>: taken from the lcc compiler.
#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))

#endif
