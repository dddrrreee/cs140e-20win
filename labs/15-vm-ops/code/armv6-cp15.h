#ifndef __ARMV6_CP15_H__
#define __ARMV6_CP15_H__
// engler, cs140e: the different data structures used by the arm coprocessor.

/**************************************************************************************
 * b4-39 / 3-60: tlb read configuration 
 */
typedef struct tlb_config {
    unsigned
        unified_p:1,    // 0:1 0 = unified, 1 = seperate I/D
        _sbz0:7,        // 1-7:7
        n_d_lock:8,       // 8-15:8  number of unified/data lockable entries
        n_i_lock:8,        //16-23:8 number of instruction lockable entries
        _sbz1:8;
} cp15_tlb_config_t;

_Static_assert(sizeof(cp15_tlb_config_t) == 4, "invalid size for struct tlb_config!");

cp15_tlb_config_t cp15_read_tlb_config(void);

// vm-helpers: print <c>
void tlb_config_print(struct tlb_config *c);


/**************************************************************************************
 * b3-12: the massive machine configuration.
 * A more full description on 3-45 in arm1176.pdf  
 * 
 * things to use for speed:
 *  - C_cache = l1 data cache
 *  - W_write_buf
 *  - Z_branch_pred
 *  - I_icache_enable
 *  - L2_enabled
 *  - C_unified_enable [note: we don't have unified I/D]
 *
 * the c1 aux register enabled branch prediction and return
 * stack prediction 3-49
 * 
 * SBO = should be 1.
 */
typedef struct control_reg1 {
    unsigned
        MMU_enabled:1,      // 0:1,   0 = MMU disabled, 1 = enabled,
        A_alignment:1,      // 1:1    0 = alignment check disabled.
        C_unified_enable:1, // 2:1    if unified used, this is enable/disable
                            // on arm1176: level 1 data cache
                            //        otherwise is enable/disable for dcache
        W_write_buf:1,      // 3:1    0 = write buffer disabled
        _unused1:3,         // 4:3
        B_endian:1,         // 7:1    0 = little 
        S_prot:1,           // 8:1   - deprecated b4-8
        R_rom_prot:1,       // 9:1   - deprecated b4-8
        F:1,                // 10:1  impl defined
        Z_branch_pred:1,    // 11:1  branch predict 0 = disabled, 1 = enabled
        I_icache_enable:1,  // 12:1  if seperate i/d, disables(0) or enables(1)
        V_high_except_v:1,  // 13:1  location of exception vec.  0 = normal
        RR_cache_rep:1,     // 14:1 cache replacement 0 = normal, 1 = different.
        L4:1,               // 15:1 inhibits arm internworking.
        _dt:1,              // 16:1, SBO
        _sbz0:1,            // 17:1 sbz
        _it:1,              // 18
        _sbz1:1,            // 19
        _st:1,              // 20
        F1:1,               // 21   fast interrupt.  [dunno]
        U_unaligned:1,      // 22 : unaligned
        XP_pt:1,            // 23:1,  1 = vmsav6, subpages disabled.
                            //    must be enabled for XN.
        VE_vect_int:1,      // 24: no.  0
        EE:1,               // 25 endient exception a2-34 
        L2_enabled:1,       // 26,
        _reserved0:1,       // 27
        TR_tex_remap:1,     // 28
        FA_force_ap:1,      // 29  0 = disabled, 1 = force ap enabled      
        _reserved1:2;
} cp15_ctrl_reg1_t;

cp15_ctrl_reg1_t cp15_ctrl_reg1_rd(void);
uint32_t cp15_ctrl_reg1_rd_u32(void);
void cp15_ctrl_reg1_wr(cp15_ctrl_reg1_t r);

/***********************************************************************************
 * set page table pointers.
 *
 * C, S, IMP, should be 0 and the offset can dynamically change based on N,
 * so we just do this manually.
 */
typedef struct {
#if 0
    unsigned 
        c:1,    // 0:1  inner cacheabled = 1, non=0
        s:1,    // 1:1, pt walk is shareable 1 or non-sharable (0)
        IMP:1,  // 2:1,
        RGN:2,  // 3:2 // B4-42: is it cacheable: 
                       // 0b00: outer non-cacheaable.  
                       // 0b10 outer write through
                       // 0b11 outer write back
        // this can change depending on what N is.  not sure the cleanest.
        base:
#endif
    unsigned base;
} cp15_tlb_reg_t;

cp15_tlb_reg_t cp15_ttbr0_rd(void);
void cp15_ttbr0_wr(cp15_tlb_reg_t r);

cp15_tlb_reg_t cp15_ttbr1_rd(void);
void cp15_ttbr1_wr(cp15_tlb_reg_t r);

// read the N that divides the address range.  0 = just use ttbr0
uint32_t cp15_ttbr_ctrl_rd(void);
// set the N that divides the address range b4-41
void cp15_ttbr_ctrl_wr(uint32_t N);

// must set both at once, AFAIK or hardware state too iffy.
struct first_level_descriptor;
void cp15_set_procid_ttbr0(uint32_t procid, struct first_level_descriptor *pt);

uint32_t cp15_procid_rd(void);

void cp15_tlbr_print(void);
void cp15_domain_print(void);


#if 0
/*******************************************************************************
 * MMU register read and writes.  
 */
// 16 2 bit access control fields.
uint32_t cp15_domain_ctrl_rd(void);
void cp15_domain_ctrl_wr(uint32_t d);

/*********************************************************************************
 * simple cache enable/disable routines.
 */

// XXX: need to figure out: if the cache is unified, can we clean it separately?
// I think not.  So better figure out if the TLB is unified or not.
void cp15_icache_inv(void);
void cp15_dcache_inv(void);
void cp15_dcache_clean_inv(void);
void cp15_caches_inv(void);

void cp15_dsb(void);
void cp15_dmb(void);

// conservative barrier for hw state modification: flush BTB, DSB, prefetch flush
void cp15_sync(void);
void cp15_barrier(void);

void cp15_tlbs_inv(void);
void cp15_dtlb_inv(void);
void cp15_itlb_inv(void);

void cp15_btb_flush(void);
void cp15_prefetch_flush(void);

// assembly code: handles all the mmu disable/enable stuff.
void mmu_disable_set_asm(cp15_ctrl_reg1_t c);
void mmu_enable_set_asm(cp15_ctrl_reg1_t c);

void mmu_get_curpid(unsigned *pid, unsigned *asid);
void mmu_asid_print(void);

#define mmu_dcache_on(r) (r).C_unified_enable = 1
#define mmu_dcache_off(r) (r).C_unified_enable = 0

#define mmu_l2cache_on(r) (r).L2_enabled = 1
#define mmu_l2cache_off(r) (r).L2_enabled = 0

#define mmu_write_buffer_on(r) (r).W_write_buf = 1
#define mmu_write_buffer_off(r) (r).W_write_buf = 0

#define mmu_icache_on(r) (r).I_icache_enable = 1
#define mmu_icache_off(r) (r).I_icache_enable = 0

#define mmu_predict_on(r) (r).Z_branch_pred = 1
#define mmu_predict_off(r) (r).Z_branch_pred = 0

#endif

#endif
