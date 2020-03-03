/*
 * engler, cs140e: receive a program over the uart
 * (you could change to use esp8266 or anything else) and run it.
 */
#include "rpi.h"
#include "rpi-asm.h"
#include "pi-message.h"
#include "libc/crc.h"

// can change these if want to use interrupt, or other methods etc.
// could make them function pointers, but this is easy.
#define get_byte uart_getc
#define put_byte uart_putc

static unsigned get_uint(void) {
	unsigned u = get_byte();
        u |= get_byte() << 8;
        u |= get_byte() << 16;
        u |= get_byte() << 24;
	return u;
}
static void put_uint(unsigned u) {
    put_byte((u >> 0)  & 0xff);
    put_byte((u >> 8)  & 0xff);
    put_byte((u >> 16) & 0xff);
    put_byte((u >> 24) & 0xff);
}

// if you want to grab large messages.
static void read_exact(void *data, unsigned n) {
    uint8_t *p = data;
    for(int i = 0; i < n; i++)
        p[i] = get_byte();
}

void notmain(void) {
    uart_init();

    // tell unix we are ready,
    put_byte(PI_GET_CODE);

    /* replay:
     *  - PI_PUT_CODE:u8
     *  - nbytes:u32, 
     *  - addr:u32
     *  - crc:u32
     *  - <nbytes of code> copy to <addr>
     */
    uint8_t op;
    uint32_t n, crc;
    void *addr = 0;
	struct pi_bin_header h; 
    AssertNow(sizeof addr == 4);

    /*
     * get the opcode, nbytes, address and crc from unix.
     * sanity check that these make sense.
     * copy the code and jump to it.
     */
	read_exact(&op, sizeof op);
	read_exact(&h, sizeof h); 
	//read_exact(&n, sizeof n);
	//read_exact(&addr, sizeof addr);
	read_exact(&crc, sizeof crc);
	
	addr = (void*)h.addr;

	if(op != PI_PUT_CODE) {
		panic("Bad opcode\n");
	}

    if(addr < (void*)HIGHEST_USED_ADDR)
        panic("invalid addr %x, must be > %x\n", addr, HIGHEST_USED_ADDR);

	n = h.nbytes;
 	
	read_exact(addr + sizeof h, n); 

	uint32_t curr_crc = our_crc32_inc(addr + sizeof h, n, our_crc32(&h, sizeof h));
	if(curr_crc != crc) {
		panic("Bad CRC: got %u, expected %u\n", our_crc32(addr + sizeof h, n), crc);
	}

    for(int i = 0; i < 10; i++) {
        printk("%d: about to call shipped code, addr=%x, nbytes=%d!\n", i,addr + sizeof h,n);
        BRANCHTO((uint32_t)(addr + sizeof h));
        printk("code returned!\n");
    }
    clean_reboot();
}
