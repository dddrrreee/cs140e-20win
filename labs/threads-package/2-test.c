/*
 * second test: simple context switching test.
 */

#include "rpi.h"
#include "rpi-thread.h"

/*
 * write these routines in 2-asm.S
 */
// *u = <x> in 2-asm.S
void store_one_asm(unsigned *u, unsigned x);
void check_callee_save(uint32_t *save);
void cswitch(uint32_t *old, uint32_t *new);

// change so that it prints out which registers are sp and lr.
void print_reg_save(uint32_t *regs) {
    demand(REG_SP_OFF != 0, you should fix this value!);
    demand(REG_LR_OFF != 0, you should fix this value!);

    unsigned off = 0;
    for(int r = 4; r <= 12; r++, off++)
        printk("offset=%d, r%d = 0x%x [%d]\n",  off, r, regs[off],regs[off]);
    printk("offset=%d, r13/sp = 0x%x [%d]\n", REG_SP_OFF, regs[REG_SP_OFF],regs[REG_SP_OFF]);
    printk("stack value should be around 0x%x, is 0x%x\n", &off, regs[REG_SP_OFF]);
    printk("offset=%d, r14/lr = 0x%x [%d]\n", REG_LR_OFF, regs[REG_LR_OFF],regs[REG_LR_OFF]);
}

void print_and_die(uint32_t *save) {
    printk("in print and die: about to dump registers from %x\n", save);
    printk("each register value except <sp> should hold its reg number (r4 holds 4, etc)\n");
    print_reg_save(save);
    clean_reboot();
}

void notmain() {
    uart_init();
    kmalloc_init();

    /*
     * part 0
     */
    printk("-----------------------------------------------------\n");
    printk("part 0: write a value into a pointer in assembly\n");
    unsigned u;
    store_one_asm(&u, 12);
    printk("u=%d\n", u);
    assert(u==12);
    store_one_asm(&u, 0xfeedface);
    assert(u==0xfeedface);
    printk("part0: SUCCESS!\n");

    printk("-----------------------------------------------------\n");
    // sizeof save area
    unsigned nbytes = sizeof ((rpi_thread_t*)0)->reg_save_area;
    uint32_t *save = kmalloc_aligned(nbytes, 8);

    /*
     * part 1: check that you can save callee registers with known values.
     * prints and dies.
     *
     * comment this out to do the next one.
     */
    printk("part1: about to test register saving\n");
    check_callee_save(save);
    panic("Should not return\n");
	clean_reboot();
}
