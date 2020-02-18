/*
 * protocol is:
 *      [GET_PROG_INFO] ----> 
 *                              <-----    [PUT_PROG_INFO, nbytes(code), crc32(code)]
 *
 *      [GET_CODE,crc32(code)]     ---->>
 *                              <----     [PUT_CODE, <nbytes of code>] 
 *
 *      [PRINT_STRING, <0-terminated string>] ----> 
 *
 *      [BOOT_SUCCESS]           ----->
 *      [BOOT_ERROR]             ----->
 */
#include <assert.h>
#include <unistd.h>

#include "libunix.h"
#include "send-recv.h"
#include "simple-boot.h"

// check that the expected value <exp> is equal the the value we <got>.
// on mismatch, // drains the tty and echos (to help debugging) and then 
// dies.
static void ck_eq32(int fd, const char *msg, unsigned exp, unsigned got) {
	if(exp == got)
        return;

    // i think there can be garbage in the initial /tty.
    output("%s: expected %x, got %x\n", msg, exp, got);

    if(fd != TRACE_FD) {
        // after error: just echo the pi output.  supposed to get a timeout.
        unsigned char b;
        while(read(fd, &b, 1) == 1) 
            fputc(b, stderr);
    }

    panic("pi-boot failed\n");
}

void simple_boot(int fd, const uint8_t *buf, unsigned n) { 
    unimplemented();
}
