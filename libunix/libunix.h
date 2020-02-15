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
char *find_ttyusb_first(void);
char *find_ttyusb_last(void);


// read in file <name>
// returns:
//  - pointer to the code.  pad code with 0s up to the
//    next multiple of 4.  
//  - bytes of code in <size>
//
// fatal error open/read of <name> fails.
uint8_t *read_file(unsigned *size, const char *name);
// same as above, but returns 0 if can't open.
uint8_t *read_file_canfail(unsigned *size, const char *name);


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

// close all open descriptors other than stdin=0, stdout=1, stderr=2
void close_open_fds(void);

// <close_open_fds> except we skip <fd> too: probably want a more general 
// implementation with a list.
void close_open_fds_except(int fd);

// return current number of usec --- probably better to make a larger datatype.
// makes printing kinda annoying however.
typedef unsigned time_usec_t;
time_usec_t time_get_usec(void);


#define close_nofail(fd) no_fail(close(fd))

// roundup <x> to a multiple of <n>: taken from the lcc compiler.
#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))


/********************************************************************************
 * general logging functions: if you are generating many log files from many 
 * executions is useful to keep in a directory numbered in order of creation.
 */

// look through the logdir for the highest log file that matches <prefix>.
//  create the next largest one.
//
// returns: the fd for the open file and the file name in <name>.
int log_create_next(const char *log_dir, const char *prefix, char *name);

// returns array pointing to all file names in <dirname> that match <prefix>
char **log_list_all(const char *dirname, const char *prefix);

// run program <cmd ...>
void run_system(const char *cmd, ...);

uint32_t crc32(const void *buf, unsigned size);

// create file <name>: truncates if already exists.
int create_file(const char *name);


/********************************************************************************
 * implement these for lab 5
 */

// create a socket pair, 
//    fork/exec program described in <argv> c
//    dup the socket to be <to_fd>
// return the other end to the client.
// write pid of forked process to <pid>
int exec_server_socket(int *pid, char **argv, int to_fd);


int child_exit_noblk(int pid, int *status);
int child_clean_exit(int pid, int *status);
int child_clean_exit_noblk(int pid, int *status);

int write_exact_can_fail(int fd, const void *data, unsigned n);
int write_exact(int fd, const void *data, unsigned n);
int read_exact(int fd, void *data, unsigned n);
int read_timeout(int fd, void *data, unsigned n, unsigned timeout);


void start_watchdog(int watch_pid);

// waits for <usec>
int can_read_timeout(int fd, unsigned usec);
// doesn't block.
int can_read(int fd);

int open_fake_tty(char **fake_dev_name);

// return a strdup's string.
char *strcatf(const char *fmt, ...);


// put your prototypes in here.
#include "libunix-extra.h"

#endif

