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

uint32_t our_crc32(const void *buf, unsigned size);

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

void simple_boot(int fd, const uint8_t *buf, unsigned n) { 
    uint32_t op;

    // if n is not a multiple of 4, use roundup() in libunix.h
    n = roundup(n,4);
    demand(n % 4 == 0, boot code size must be a multiple of 4!);

    output("******************sending %d bytes\n", n);

    // 0. we drain the initial pipe: can have garbage.   it's a little risky to
    // use <get_op> since if the garbage matches the opcode, we will print
    // nonsense.  could add a crc.
    while((op = get_op(fd)) != GET_PROG_INFO) {
        output("expected initial GET_PROG_INFO, got <%x>: discarding.\n", op);
    }

    //output("received GET_PROG_INFO \n");
    // 1. reply to the GET_PROG_INFO
    
    put_uint32(fd, PUT_PROG_INFO);
    put_uint32(fd, ARMBASE);
    put_uint32(fd, n);
    
    uint32_t checksum = our_crc32(buf, n);
    put_uint32(fd, checksum);

    // 2. drain any extra GET_PROG_INFOS
    while((op = get_op(fd)) == GET_PROG_INFO){
        ;
    }

    // 3. check that we received a GET_CODE
    /*
    uint32_t checkval = get_uint32(fd);
    while(checkval != GET_CODE) {
        checkval = get_uint32(fd);
    }
    */
    
    
    if(op != GET_CODE) {
        die("Not the right code\n");
    }
    
    //ck_eq32(fd, "Not the right code", op, GET_CODE);

    //output("received GET_CODE: %X \n", checkval);
    
    if(get_uint32(fd) != checksum) {
        die("Checksums don't match\n");
    }
    
    //ck_eq32(fd, "Checksums don't match", checksum, get_uint32(fd));
    //output("Checksums match!\n");

    // 4. handle it: send a PUT_CODE massage.
    put_uint32(fd, PUT_CODE);
    for(int i = 0; i < n; i++) {
        put_byte(fd, buf[i]);
    }
    //output("sent binary \n");
    
    // 5. Wait for success
    if(get_op(fd) != BOOT_SUCCESS) {
        die("BOOT SUCCESS not received\n");
    }
    //ck_eq32(fd, "BOOT_SUCCESS mismatch", BOOT_SUCCESS, get_op(fd));
    ck_eq32(fd, "shouldn't fail",  1, 1);
    output("bootloader: Done.\n");
}
