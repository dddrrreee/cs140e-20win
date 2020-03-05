#ifndef __ARM_COPROCESSOR_INSTS_H__
#define __ARM_COPROCESSOR_INSTS_H__
/*
 * engler, cs140e: a very partial set of the co-processor instructions.
 * the is an outrageous abundance of them in ch 3 in arm1176: a good 
 * final project is packaging them up and making an interesting subsystem.
 *
 * this header just contains raw instructions.  the cookbook sequences from
 * the arm manual are mainly in your-vm-asm.S (written by you) and mmu-asm.h
 */

/************************************************************************
 * b4-39-
 * all vmsa-related registers are accessed with instructions of the form:
 *   mrc p15, 0, Rd, CRn, CRm, opcode_2
 *   mcr p15, 0, Rd, CRn, CRm, opcode_2
 *
 * where CRn is the system co-processor registered.   Unless otherwise specified,
 * CRm and opcode_2 SBZ (should be zero).  [You can see how this plays out below]
 *
 */

/* 
 * used to be called "drain write buffer"
 * includes all cache operations.  is a superset (>) DMB
 */
#define DSB(Rd)             mcr p15, 0, Rd, c7, c10, 4
#define DMB(Rd)             mcr p15, 0, Rd, c7, c10, 5 

/*
 * must flush the prefetch buffer whenever you change a virtual
 * mapping (ASID, PTE, etc) since it will have stale instructions.
 *
 * if you are doing this, likely have to do a DSB before to make
 * sure whatever invalidation you did has completed.
 */
#define PREFETCH_FLUSH(Rd)  mcr p15, 0, Rd, c7, c5, 4  
/* must do this after changing any MMU stuff, ASID, etc. */
#define FLUSH_BTB(Rd)         mcr p15, 0, Rd, c7, c5, 6

/*
 * Work-around for bug in ARMv6 if we have seperate I/D.  Taken from:
 *   https://elixir.bootlin.com/linux/latest/source/arch/arm/mm/cache-v6.S
 * MUST HAVE INTERRUPTS DISABLED!
 * XXX: patch feedback implies we need this for other operations too?
 */
#define INV_ICACHE(Rd)                                           \
    mcr p15, 0, Rd, c7, c5, 0   ; /* invalidate entire I-cache */   \
    mcr p15, 0, Rd, c7, c5, 0;  ; /* invalidate entire I-cache */   \
    mcr p15, 0, Rd, c7, c5, 0;  ; /* invalidate entire I-cache */   \
    mcr p15, 0, Rd, c7, c5, 0;  ; /* invalidate entire I-cache */   \
    .rept   11                  ; /* ARM Ltd recommends at least 11 nops */\
    nop                         ;                                   \
    .endr

/*
 * arm1176.pdf 3-74
 * If it is essential that the cache is clean, or clean and invalid, for
 * a particular operation, the sequence of instructions for cleaning,
 * or cleaning and invalidating, the cache for that operation must
 * handle the arrival of an interrupt at any time when interrupts are not
 * disabled. This is because interrupts can write to a previously clean
 * cache. For this reason, the Cache Dirty Status Register indicates
 * if the cache has been written to since the last clean of the cache
 * was started, see Cache Dirty Status Register on page 3-78. You can
 * interrogate the Cache Dirty Status Register to determine if the cache
 * is clean, and if this is done while interrupts are disabled, the
 * following operations can rely on having a clean cache.  The following
 * sequence shows this approach:
 */
#define CLEAN_INV_DCACHE(Rd)    mcr p15, 0, Rd, c7, c14, 0  
#define INV_DCACHE(Rd)          mcr p15, 0, Rd, c7, c6, 0  


// Note: I'm inclined to believe the icache bug above effects the arm invalidate
// all caches operation too, so we inv both I/D separately rather than use:
//      #define INV_ALL_CACHES(Rd)  mcr p15, 0, Rd, c7, c7, 0  
#define INV_ALL_CACHES(Rd) INV_ICACHE(Rd); INV_DCACHE(Rd)

/*
 *
 * b4-45 --- tlb control functions. cr8 is a write only register used to
 * control tlbs.  this is a subset.
 * read b2-22: TLB maintance operations and the memory order model.
 *
 * there are other functions, such as invalidate asid and invalidate
 * a single entry.  we may do later.
 *
 * i think we have to invalidate the BTB and Prefetch if doing these.
 */

#define INV_ITLB(Rd)        mcr p15, 0, Rd, c8, c5, 0 
#define INV_DTLB(Rd)        mcr p15, 0, Rd, c8, c6, 0 
/* invalidate unified TLB or both I/D TLB */
#define INV_TLB(Rd)         mcr p15, 0, Rd, c8, c7, 0

/*
 * 3-61 in arm1176.pdf
 * almost certainly do not want to do the write (*_SET) operations standalone.  
 * there's a complicated little dance you must obey.
 */
#define TTBR0_GET(Rd)           mrc p15, 0, Rd, c2, c0, 0
#define TTBR0_SET(Rd)           mcr p15, 0, Rd, c2, c0, 0
#define TTBR1_GET(Rd)           mrc p15, 0, Rd, c2, c0, 1
#define TTBR1_SET(Rd)           mcr p15, 0, Rd, c2, c0, 1
#define TTBR_BASE_CTRL_RD(Rd)   mrc p15, 0, Rd, c2, c0, 2
#define TTBR_BASE_CTRL_WR(Rd)   mcr p15, 0, Rd, c2, c0, 2

// b4-52: 
//   - context id: has both the procid (24-bits, defined by user)
// and 
//   - asid: 64.  must reserve asid=0.
#define ASID_SET(Rd)            mcr p15, 0, Rd, c13, c0, 1
#define ASID_GET(Rd)            mrc p15, 0, Rd, c13, c0, 1

/* arm1176 3-63 */
#define DOMAIN_CTRL_RD(Rd)      mrc p15, 0, Rd, c3, c0, 0
#define DOMAIN_CTRL_WR(Rd)      mcr p15, 0, Rd, c3, c0, 0

/* from b6-13: no WR method, correct? */
#define CACHE_TYPE_RD(Rd)       mrc p15, 0, Rd, c0, c0, 1
#define TLB_CONFIG_RD(Rd)       mrc p15, 0, Rd, c0, c0, 3

/* b4-40 */
#define CONTROL_REG1_RD(Rd) mrc p15, 0, Rd, c1, c0, 0
#define CONTROL_REG1_WR(Rd) mcr p15, 0, Rd, c1, c0, 0


// b4-43
#define FAULT_STATUS_REG_GET(Rd) mrc p15, 0, Rd, c5, c0, 0

/*
 * many things not implemented: fault status, b4-43, watch point b4-44.  
 * extension: fill these in!
 */
#endif
