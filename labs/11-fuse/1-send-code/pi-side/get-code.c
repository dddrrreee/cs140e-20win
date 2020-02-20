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
    AssertNow(sizeof addr == 4);

    /*
     * get the opcode, nbytes, address and crc from unix.
     * sanity check that these make sense.
     * copy the code and jump to it.
     */ 
    unimplemented();

    if(addr < (void*)HIGHEST_USED_ADDR)
        panic("invalid addr %x, must be > %x\n", addr, HIGHEST_USED_ADDR);

    unimplemented();

    for(int i = 0; i < 10; i++) {
        printk("%d: about to call shipped code, addr=%x, nbytes=%d!\n", i,addr,n);
        BRANCHTO((uint32_t)addr);
        printk("code returned!\n");
    }
    clean_reboot();
}
