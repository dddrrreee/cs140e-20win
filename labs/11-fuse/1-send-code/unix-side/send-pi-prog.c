// simple unix side to send a program to the pi and have it run it.
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

#include "rpi.h"
#include "rpi-asm.h"
#include "libunix.h"
#include "pi-message.h"

void put_byte(int fd, uint8_t b) {
    // slow it down.
    int r;
    if((r = write(fd, &b, 1)) != 1)
        sys_die(write, "write() failed: ret=%d\n", r);
}

uint8_t get_byte(int fd) {
    uint8_t b;
    int n;
    if((n = read(fd, &b, 1)) == 1)
        return b;

    if(!n)
        panic("pi-install: read() timed-out: pi not listening --- power-cycle?\n");
    sys_die(read, "pi-install: read() failed: expected 1 byte, got %d\n", n);
}

// note: the other way to do is to assign these to a char array b and
//    return *(unsigned)b
// however, the compiler doesn't have to align b to what unsigned
// requires, so this can go awry.  easier to just do the simple way.
// we do with |= to force get_byte to get called in the right order
//     (get_byte(fd) | get_byte(fd) << 8 ...)
// isn't guaranteed to be called in that order b/c '|' is not a seq point.
uint32_t get_uint32(int fd) {
    uint32_t u;
    u  = get_byte(fd);
    u |= get_byte(fd) << 8;
    u |= get_byte(fd) << 16;
    u |= get_byte(fd) << 24;
    return u;
}

void put_uint32(int fd, uint32_t u) {
    // NOTE: mask not necessary; just cosmetic
    put_byte(fd, (u >> 0)  & 0xff);
    put_byte(fd, (u >> 8)  & 0xff);
    put_byte(fd, (u >> 16) & 0xff);
    put_byte(fd, (u >> 24) & 0xff);
}


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

    read_exact(0, &cookie, sizeof cookie);
    uint32_t expect = 0x12345678;
    if(cookie != expect)
        panic("invalid cookie: %x, expected %x\n", cookie, expect);
    
    read_exact(0, &addr, sizeof(addr));
    
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
    put_byte(fd, PI_PUT_CODE);
    put_uint32(fd, n);
    put_uint32(fd, addr);
    put_uint32(fd, our_crc32(buf, n));

	int i;
	for(i = 0; i < n; ++i) {
		put_byte(fd, buf[i]);
	}
	//write_exact(fd, buf, n);

    output("loaded code at %x with nbytes=%d: going to echo output\n", addr,n);
    pi_echo(0, fd, 0);
    return 0;
}
