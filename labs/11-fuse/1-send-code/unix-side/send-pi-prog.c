// simple unix side to send a program to the pi and have it run it.
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

#include "rpi.h"
#include "rpi-asm.h"
#include "libunix.h"
#include "pi-message.h"

// have them run a simple prog.  can we feed stdin to it?
int main(int argc, char *argv[]) {
    int fd = HANDOFF_FD;
    if(!fd_is_open(fd))
        panic("%s: must run with <HANDOFF_FD> as an open file descriptor\n", argv[0]);

    // wait for the pi to signal it is ready.
    output("about to wait for pi ack\n");
    // wait until the pi is ready.
    uint8_t op;
    read_exact(fd, &op, 1);
    if(op != PI_GET_CODE)
        panic("expected %d, have %d\n", PI_GET_CODE,op);

    output("have pi ack: going to read code\n");

    // read in the file from stdin.
    // first word better be the magic cookie.
    // second word better be a sensible link address.
    //
    // send:
    //   PI_PUT_CODE:u8
    //   nbytes: u32
    //   addr : u32
    //   crc : u32 
    // to the pi.  make sure you don't have the pi jump to the header address
    // in the binary!  must jump to the code.  look at the memmap/.list file
    // for hello-fixed
    //
    // after sending: run pi_echo.
    char buf[1024 * 64];
    uint32_t cookie, addr;

    read_exact(0, &cookie, sizeof addr);
    uint32_t expect = 0x12345678;
    if(cookie != expect)
        panic("invalid cookie: %x, expected %x\n", cookie, expect);

    unimplemented();


    if(addr <= HIGHEST_USED_ADDR)
        panic("specified addr %x will hit pi code %x\n", 
                addr, HIGHEST_USED_ADDR);

    // read in code, check for errors.
    int n = read(0, buf, sizeof buf);
    if(n < 0)
        sys_die(read, weird error);
    if(n < 512)
        panic("unlikely size: %d bytes\n", n);
    if(n == sizeof buf)
        panic("overflowed buffer: increase from %lu bytes\n", sizeof buf);

    // send to the pi!
    unimplemented();

    output("loaded code at %x with nbytes=%d: going to echo output\n", addr,n);
    pi_echo(0, fd, 0);
    return 0;
}
