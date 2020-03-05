/* engler, 140e: sorta cleaned up test code from 14-vm.  */
#include "rpi.h"
#include "rpi-constants.h"
#include "rpi-interrupts.h"

#include "mmu.h"

static const unsigned OneMB = 1024 * 1024 ;

// for today, just crash and burn if we get a fault.
void data_abort_vector(unsigned lr) {
    unsigned fault_addr;
    // b4-44
    asm volatile("MRC p15, 0, %0, c6, c0, 0" : "=r" (fault_addr));
    mmu_disable();
    panic("data_abort fault at %x\n", fault_addr);
}

// shouldn't happen: need to fix libpi so we don't have to do this.
void interrupt_vector(unsigned lr) {
    mmu_disable();
    panic("impossible\n");
}

/*
 * an ugly, trivial test run.
 *  1. setup the virtual memory sections.
 *  2. enable the mmu.
 *  3. do some simple tests.
 *  4. turn the mmu off.
 */ 
void vm_test(void) {

    // 1. init
    mmu_init();
    assert(!mmu_is_enabled());

    void *base = (void*)0x100000;
    fld_t *pt = mmu_pt_alloc_at(base, 4096*4);
    assert(pt == base);

    // we use a non-zero domain id to test things out.
    unsigned dom_id = 1;

    // 2. setup mappings

    // map the first MB: shouldn't need more memory than this.
    mmu_map_section(pt, 0x0, 0x0, dom_id);
    // map the page table: for lab cksums must be at 0x100000.
    mmu_map_section(pt, 0x100000,  0x100000, dom_id);
    // map stack (grows down)
    mmu_map_section(pt, STACK_ADDR-OneMB, STACK_ADDR-OneMB, dom_id);

    // map the GPIO: make sure these are not cached and not writeback.
    // [how to check this in general?]
    mmu_map_section(pt, 0x20000000, 0x20000000, dom_id);
    mmu_map_section(pt, 0x20100000, 0x20100000, dom_id);
    mmu_map_section(pt, 0x20200000, 0x20200000, dom_id);

    // if we don't setup the interrupt stack = super bad infinite loop
    mmu_map_section(pt, INT_STACK_ADDR-OneMB, INT_STACK_ADDR-OneMB, dom_id);

    // 3. install fault handler to catch if we make mistake.
    mmu_install_handlers();

    // 4. start the context switch:
     
    // set up permissions for the different domains.
    write_domain_access_ctrl(0b01 << dom_id*2);

    // use the sequence on B2-25
    set_procid_ttbr0(0x140e, dom_id, pt);

    // note: this routine has to flush I/D cache and TLB, BTB, prefetch buffer.
    mmu_enable();

    /*********************************************************************
     * 5. trivial test: write a couple of values, make sure they succeed.
     *
     * you should write additional tests!
     */
    assert(mmu_is_enabled());

    // read and write a few values.
    int x = 5, v0, v1;
    assert(get32(&x) == 5);

    v0 = get32(&x);
    printk("doing print with vm ON\n");
    x++;
    v1 = get32(&x);

    /*********************************************************************
     * done with trivial test, re-enable.
     */

    // 6. disable.
    mmu_disable();
    assert(!mmu_is_enabled());
    printk("OFF\n");

    // 7. make sure worked.
    assert(v0 == 5);
    assert(v1 == 6);
    printk("******** success ************\n");
}

void notmain() {
    uart_init();
    printk("implement one at a time.\n");
    check_vm_structs();
    vm_test();
    clean_reboot();
}
