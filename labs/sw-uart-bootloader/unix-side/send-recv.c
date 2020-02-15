#include <unistd.h>

#include "libunix.h"
#include "send-recv.h"

void put_byte(int fd, uint8_t b) {
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
//	return *(unsigned)b
// however, the compiler doesn't have to align b to what unsigned 
// requires, so this can go awry.  easier to just do the simple way.
// we do with |= to force get_byte to get called in the right order 
// 	(get_byte(fd) | get_byte(fd) << 8 ...) 
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
