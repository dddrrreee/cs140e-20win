/* 
 * engler, cs140e: simple unix-side bootloader implementation.  
 * see the lab for the protocol definition.
 */

/*********************************************************************************
 * Do not modify the code below.
 */
#include <assert.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "libunix.h"
#include "send-recv.h"

#define __SIMPLE_IMPL__
#include "../shared-code/simple-boot.h"

// check that the expected value <exp> is equal the the value we <got>.
// on mismatch, // drains the tty and echos (to help debugging) and then 
// dies.
static void ck_eq32(int fd, const char *msg, unsigned exp, unsigned got) {
	if(exp == got)
        return;

    // i think there can be garbage in the initial /tty.
    output("%s: expected %x, got %x\n", msg, exp, got);

    // after error: just echo the pi output.
    unsigned char b;
    while(read(fd, &b, 1) == 1) 
        fputc(b, stderr);

    panic("pi-boot failed\n");
}

// hack to handle unexpected PRINT_STRINGS: call <get_op> for the
// first uint32 in a message.  the code checks if it received a 
// PRINT_STRING and emits if so, otherwise returns the value.
uint32_t get_op(int fd) {
    uint32_t op = get_uint32(fd);
    if(op != PRINT_STRING)
        return op;

    unsigned nbytes = get_uint32(fd);
    demand(nbytes < 512, pi sent a suspiciously long string);
    output("pi sent print: <");
    for(int i = 0; i < nbytes; i++)
        output("%c", get_byte(fd));
    output(">\n");
    return get_op(fd);
}

/*********************************************************************************
 * Put your modifications below.
 */

// Implement steps
//    1,2,3,4.
//
//  0 and 5 are implemented for exaple.
//
// Note: if timeout is too small you can fail here.
void simple_boot(int fd, const uint8_t *buf, unsigned n) { 
    uint32_t op;

    // if n is not a multiple of 4, use roundup() in libunix.h
    n = roundup(n,4);
    demand(n % 4 == 0, boot code size must be a multiple of 4!);

    output("******************sending %d bytes\n", n);

    // 0. we drain the initial pipe: can have garbage.   it's a little risky to
    // use <get_op> since if the garbage matches the opcode, we will print
    // nonsense.  could add a crc.
    while((op = get_op(fd)) != GET_PROG_INFO)
        output("expected initial GET_PROG_INFO, got <%x>: discarding.\n", op);

    // 1. reply to the GET_PROG_INFO
    unimplemented();

    // 2. drain any extra GET_PROG_INFOS
    unimplemented();

    // 3. check that we received a GET_CODE
    unimplemented();

    // 4. handle it: send a PUT_CODE massage.
    unimplemented();

    // 5. Wait for success
    ck_eq32(fd, "BOOT_SUCCESS mismatch", BOOT_SUCCESS, get_op(fd));
    output("bootloader: Done.\n");
}
