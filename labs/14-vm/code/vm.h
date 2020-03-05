#ifndef __VM_H__
#define __VM_H__
/****************************************************************************************
 * engler, cs140e: structures for armv6 virtual memory.
 *
 * ./docs/README.md  gives pages numbers for (some) key things if you can't 
 * find them.
 *
 * page table operations: we only handle mapping 1MB sections.
 */

// need to add citations to the various things, especially for the 
// different routines.

/*
    -----------------------------------------------------------------
    b4-26 first level descriptor  
        bits0:1 =
            - 0b00: unmapped, 
            - 0b10: section descriptor, 
            - 0b01: second level coarse pt.
            - 0b11: reserved

    b4-27 section:
        31-20: section base address: must be aligned.
        19: sbz: "should be zero"
        18: 0
        17: nG:  b4-25, global bit=0 implies global mapping, g=1, process 
                 specific.
        -16: S: 0 deprecated.
        -15: APX  0b11 r/w [see below]
        -14-12: TEX: table on b4-12
        -11-10: AP 
        9: IMP: 0: should be set to 0 unless the implementation defined
                   functionality is required.

        -8-5: Domain: b4-10: 0b11 = manager (no perm check), 0b01 (checked perm)
        -4: XN: 1 = execute never,  0 = can execute.
        3:C: set 0 see below
        2:B: set 0
        1: 1
        0: 0

  APX, S, R: 
    b4-8: permissions, table on b4-9
    use of S/R deprecated.

    S  R  APX   AP[1:0]   privileged permissions   user permissions
    x  x    0   0b11       r/w                       r/w
    x  x    0   0b10       r/w                       r/o
    x  x    0   0b01       r/w                       no access
    0  0    0      0       no access                no access
    
  ap,apx,domain:
     b4-8

  c,b,tex:
   b4-11, b4-12 

    TEX   C  B 
    0b000 0  0 strongly ordered.   
    0b001 0  0 non-cacheable
*/

// on pi: organized from bit 0 to high.
typedef struct first_level_descriptor {
    unsigned
        tag : 2,      // 0-1:2    should be 0b10
        B : 1,        // 2:1      set to 0
        C : 1,        // 3:1      set to 0
        XN : 1,       // 4:1      1 = execute never, 0 = can execute
                    // needs to have XP=1 in ctrl-1.

        domain : 4,   // 5-8:4    b4-10: 0b11 = manager, 0b01 checked perms
        IMP : 1,      // 9:1      should be set to 0 unless imp-defined 
                    //          functionality is needed.

        AP : 2,       // 10-11:2  permissions, see b4-8/9
        TEX : 3,      // 12-14:3
        APX : 1,      // 15:1     
        S : 1,        // 16:1     set=0, deprecated.
        nG : 1,       // 17:1     nG=0 ==> global mapping, =1 ==> process specific
        super : 1,    // 18:1     selects between section (0) and supersection (1)
        _sbz1 : 1,    // 19:1     sbz
        sec_base_addr : 12; // 20-31.  must be aligned.
} fld_t;
// _Static_assert(sizeof(fld_t) == 4, "invalid size for fld_t!");

// vm-helpers: print <f>
void fld_print(fld_t *f);

// make a default fld: tag must be <0b10>
fld_t fld_mk(void);

/**************************************************************************************
 * b4-39: tlb read configuration 
 */

// 3-60
// b4-39
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
_Static_assert(sizeof(cp15_ctrl_reg1_t) == 4, "invalid size for ctrl_reg1_t!");

cp15_ctrl_reg1_t cp15_ctrl_reg1_rd(void);
void cp15_ctrl_reg1_wr(cp15_ctrl_reg1_t r);

// vm-helpers.c: print out <r>
void control_reg1_print(struct control_reg1 *r);





/***************************************************************************
 * ttbr0,ttbr1,ttbrc: arm1176/6-42
 *
 * registers that point to the lower page table (ttbr0), the upper if any (ttbr1).
 * set using ttbrc
 */

// C, S, IMP, should be 0 and the offset can dynamically change based on N,
// so we just do this manually.
struct tlb_reg {
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
};

struct tlb_reg read_ttbr0(void);
void write_ttbr0(struct tlb_reg r);

struct tlb_reg read_ttbr1(void);
void write_ttbr1(struct tlb_reg r);

// read the N that divides the address range.  0 = just use ttbr0
unsigned read_tlb_base_ctrl(void);
// set the N that divides the address range b4-41
void write_tlb_base_ctrl(unsigned N);

/**************************************************************************
 * simplistic cache coherence, tlb, mmu, cache flushing code.  this only
 * works b/c in today's lab we are running uncached and not reusing entries.
 */
void clean_inv_caches(void);


/***************************************************************************
 * below are the routines you have to implement eventually (not today)
 */

unsigned read_domain_reg(void);
void write_domain_reg(unsigned);

unsigned tlb_get_procid(void);
unsigned our_tlb_get_procid(void);

// lower 8 bits are ASID
void tlb_set_procid(unsigned proc_id);
void our_tlb_set_procid(unsigned proc_id);

unsigned get_cache_type(void);
unsigned our_get_cache_type(void);

void set_procid_ttbr0(unsigned procid, fld_t *pt);
void our_set_procid_ttbr0(unsigned procid, fld_t *pt);

void mmu_enable(cp15_ctrl_reg1_t c);
void our_mmu_enable(cp15_ctrl_reg1_t c);

void mmu_disable(struct control_reg1 off);
void our_mmu_disable(struct control_reg1 off);

// 16 2 bit access control fields.
unsigned read_domain_access_ctrl(void);
void our_write_domain_access_ctrl(unsigned d);

// called to sync after a pte modification.
void mmu_sync_pte_mod(void);

/***********************************************************************
 * utility routines.
 */

// hash [data, data+n) and print it with <msg>
void hash_print(const char *msg, const void *data, unsigned n);

// vm-helpers.c: one-time sanity check of the offsets in the structures above.
void check_vm_structs(void);

//fld_t * our_mmu_map_section(fld_t *pt, unsigned va, unsigned pa);
//fld_t *our_mmu_init(unsigned base);
//void our_mmu_mark_sec_ap_perm(fld_t *pt, unsigned va, unsigned nsec, unsigned perm);


#endif
