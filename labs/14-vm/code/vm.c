/*
	engler, 140e: simple code for VM: starter code at the top (mostly).  
    - your code at the end.  
    - write your own implementation of any our_* function.

    b4-6: before mmu enabled, the icache should be disabled and invalidated.
     icache can then be enabled at the same time as the mmu is enabled.
     clean_inv_caches();

    b4-7: strongly recommended that the code which enables or disables the 
     mmu has identical virtual and physical addresses.
*/
#include "rpi.h"
#include "rpi-constants.h"
#include "vm.h"

static const unsigned OneMB = 1024 * 1024 ;

static unsigned sec_bits_only(unsigned u) {
    return u & ~((1<<21)-1);
}

#define is_aligned(x, a)        (((x) & ((typeof(x))(a) - 1)) == 0)

/*************************************************************************
 * tlbr  b4-41
 */
void ttbr0_get(unsigned *base, unsigned *rgn, unsigned n) {
    assert(n == 0);
    // assert(read_tlb_base_ctrl() == 0);

    unsigned r = read_ttbr0().base;
    *rgn = (r >> 3) & 0b11;
    *base = (r >> 14) << 14;

}
void ttbr1_get(unsigned *base, unsigned *rgn, unsigned n) {
    assert(n == 0);
    //assert(read_tlb_base_ctrl() == 0);

    unsigned r = read_ttbr1().base;
    *rgn = (r >> 3) & 0b11;
    *base = (r >> 14) << 14;
    // assert(*base == 0);
}

static void tlbr_print(void) {
    unsigned base0, rgn0, base1, rgn1;
    ttbr0_get(&base0, &rgn0, 0);
    ttbr1_get(&base1, &rgn1, 0);
    printk("TLBR registers:\n");
    printk("\ttlb base control: N=%b\n", read_tlb_base_ctrl());
    printk("\tTTBR0: base=%x, rgn=%b\n", base0, rgn0);
    printk("\tTTBR1: base=%x, rgn=%b\n", base1, rgn1);
}

/*************************************************************************
 * domain access control  b4-42
 */

void domain_acl_print(void) {
    printk("domain access control:\n");
    printk("\t%b\n", read_domain_access_ctrl());
}


/************************************************************************
 * Part1: implement FLD.   vm-helpers.c has checking and printing helpers.
 */

fld_t * mmu_lookup_section(fld_t *pt, unsigned va) {
    fld_t *pte = 0;
    unimplemented();
    demand(pte->tag, invalid section);
    return pte;
}

// B4-9: AP field:  no/access=0b00, r/o=0b10, rw=0b11
enum {
    // read-write access
    AP_rw = 0b11,
    // no access either privileged or user
    AP_no_access = 0b00,
    // privileged read only.
    AP_rd_only = 0b10
};

// mark sections [sec,sec+nsec) by setting pte->AP = <perm>
void mmu_mark_sec_ap_perm(fld_t *pt, unsigned va, unsigned nsec, unsigned perm) {
    unimplemented();
}
void mmu_mark_sec_readonly(fld_t *pt, unsigned va, unsigned nsec) {
    our_mmu_mark_sec_ap_perm(pt,va,nsec, AP_rd_only);
}
void mmu_mark_sec_rw(fld_t *pt, unsigned va, unsigned nsec) {
    our_mmu_mark_sec_ap_perm(pt,va,nsec, AP_rw);
}
void mmu_mark_sec_no_access(fld_t *pt, unsigned va, unsigned nsec) {
    our_mmu_mark_sec_ap_perm(pt,va,nsec, AP_no_access);
}

// create a mapping for <va> to <pa> in the page table <pt>
// for now: 
//  - assume domain = 0.
//  - global mapping
//  - executable
//  - don't use APX
//  - TEX,A,B: for today mark everything as non-cacheable 6-15/6-16 in armv1176
fld_t * mmu_map_section(fld_t *pt, unsigned va, unsigned pa) {
    assert(is_aligned(va, 20));
    assert(is_aligned(pa, 20));

    unimplemented();
}

void mmu_map_sections(fld_t *pt, unsigned va, unsigned pa, unsigned nsec) {
    assert(is_aligned(va, 20));
    assert(is_aligned(pa, 20));
    for(unsigned i = 0; i < nsec; i++) {
        mmu_map_section(pt, va,pa);
        va += OneMB;
        pa += OneMB;
    }
}

// would want to initialize everything.
fld_t *mmu_init(unsigned base) {
    demand(is_aligned(base, 14), must be 14 bit aligned!);
    fld_t *pt = (void*)base;

    unimplemented();
    return pt;
}

// part 2: define this and then implement everything.
//#define PART2_FAULT_TEST
#ifdef PART2_FAULT_TEST

// we do this in a different way: rather than do the full interrupt setup, etc,
// just write the jump to the handler location.
#include "rpi-interrupts.h"
// call this before calling panic
void x_mmu_disable(void) {
    cp15_ctrl_reg1_t c1 = cp15_ctrl_reg1_rd();
    c1.MMU_enabled = 0;
    our_mmu_disable(c1);
}

void prefetch_abort_vector(unsigned lr) {
    x_mmu_disable();
    panic("in data prefetch\n");
}

// b4-20
enum {
    SECTION_XLATE_FAULT = 0b00101,
    SECTION_PERM_FAULT = 0b01001,
};

// remove all non-section bits.

void data_abort_vector(unsigned lr) {
    x_mmu_disable();
    panic("in data abort\n");
}

void interrupt_vector(unsigned lr) {
    x_mmu_disable();
    panic("impossible\n");
}
void mmu_install_handlers(void) {
    int_init();
}
#endif


/*
 * an ugly, trivial test run.
 *  1. setup the virtual memory sections.
 *  2. enable the mmu.
 *  3. do some simple tests.
 *  4. turn the mmu off.
 *
 * because we have just booted up and we don't switch back and forth between
 * addresss spaces we can be very sleazy about flushing hardware state to 
 * keep things consistent.  next lab does things right.
 */ 
void vm_test(void) {
    printk("******** part 1! ************\n");
    unsigned base = 0x100000;

    fld_t *pt = our_mmu_init(base);
    assert(pt == (void*)0x100000);

    // map the first MB: shouldn't need more memory than this.
    our_mmu_map_section(pt, 0x0, 0x0);
    // map the page table: for lab cksums must be at 0x100000.
    our_mmu_map_section(pt, 0x100000,  0x100000);
    // map stack (grows down)
    our_mmu_map_section(pt, STACK_ADDR-OneMB, STACK_ADDR-OneMB);

    // map the GPIO: make sure these are not cached and not writeback.
    // [how to check this in general?]
    our_mmu_map_section(pt, 0x20000000, 0x20000000);
    our_mmu_map_section(pt, 0x20100000, 0x20100000);
    our_mmu_map_section(pt, 0x20200000, 0x20200000);

#ifdef PART2_FAULT_TEST
    our_mmu_map_section(pt, INT_STACK_ADDR-OneMB, INT_STACK_ADDR-OneMB);
    mmu_install_handlers();
#endif

    // this should be wrapped up neater.  broken down so can replace 
    // one by one.
    //  1. the fields are defined in vm.h.
    //  2. specify armv6 (no subpages).
    //  3. check that the coprocessor write succeeded.
    struct control_reg1 c1 = cp15_ctrl_reg1_rd();
    c1.XP_pt = 1;
    cp15_ctrl_reg1_wr(c1);
    c1 = cp15_ctrl_reg1_rd();
    assert(c1.XP_pt);
    assert(!c1.MMU_enabled);

    // write all the domain's to full access (next time do something better).
    our_write_domain_access_ctrl(~0UL);
    dsb();

    // use the sequence on B2-25
    our_set_procid_ttbr0(1, pt);

    // have to flush I/D cache and TLB, BTB, prefetch buffer.
    c1.MMU_enabled = 1;
    our_mmu_enable(c1);

    // double check that 
    //  VM ON!
    c1 = cp15_ctrl_reg1_rd();
    assert(c1.MMU_enabled);

    // read and write a few values.
    int x = 5, v0, v1;

    assert(get32(&x) == 5);

    v0 = get32(&x);
    printk("doing print with vm ON\n");
    x++;
    v1 = get32(&x);


#ifdef PART2_FAULT_TEST
    // 1. do an out-of-bounds stack write.

    // we know this is not mapped.
    volatile uint8_t *p = (void*)(STACK_ADDR - OneMB - 16);
    printk("about to do an invalid write to %x\n", p);
    *p = 11;
    assert(*p == 11);
    printk("done with invalid write to %x have=%d\n", p,get32(p));

    // 2. do a write with insufficient privleges.
    printk("about to mark no access%x\n", p);
    mmu_mark_sec_no_access(pt, (unsigned)p, 1);
    *p = 12;
    assert(*p == 12);
    printk("done with invalid perm write to %x have=%d\n", p,*p);

    printk("PART2: success!\n");
#endif

    // Turn MMU off.
    c1.MMU_enabled = 0;
    our_mmu_disable(c1);

    // verify it's disabled.
    c1 = cp15_ctrl_reg1_rd();
    assert(!c1.MMU_enabled);
    printk("OFF\n");

    // our reads worked.
    assert(v0 == 5);
    assert(v1 == 6);
    printk("******** success ************\n");
}

void notmain() {
    uart_init();
    printk("implement one at a time.\n");

    printk("this is part0\n");
    check_vm_structs();
    printk("SUCCESS1\n");

    printk("this is part1\n");
    vm_test();

    clean_reboot();
}
