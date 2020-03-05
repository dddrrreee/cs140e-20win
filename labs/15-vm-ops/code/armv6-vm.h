#ifndef __ARM_VM_H__
#define __ARM_VM_H__

// get the different cp15 data structures.
// #include "cp15-arm.h"

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
        tag:2,      // 0-1:2    should be 0b10
        B:1,        // 2:1      set to 0
        C:1,        // 3:1      set to 0
        XN:1,       // 4:1      1 = execute never, 0 = can execute
                    // needs to have XP=1 in ctrl-1.

        domain:4,   // 5-8:4    b4-10: 0b11 = manager, 0b01 checked perms
        IMP:1,      // 9:1      should be set to 0 unless imp-defined 
                    //          functionality is needed.

        AP:2,       // 10-11:2  permissions, see b4-8/9
        TEX:3,      // 12-14:3
        APX:1,      // 15:1     
        S:1,        // 16:1     set=0, deprecated.
        nG:1,       // 17:1     nG=0 ==> global mapping, =1 ==> process specific
        super:1,    // 18:1     selects between section (0) and supersection (1)
        _sbz1:1,    // 19:1     sbz
        sec_base_addr:12; // 20-31.  must be aligned.
} fld_t;
_Static_assert(sizeof(fld_t) == 4, "invalid size for fld_t!");

// B4-9: AP field:  no/access=0b00, r/o=0b10, rw=0b11
enum {
    // read-write access
    AP_rw = 0b11,
    // no access either privileged or user
    AP_no_access = 0b00,
    // privileged read only.
    AP_rd_only = 0b10
};

// b4-10
enum {  
    DOM_no_access = 0b00, // any access = fault.
    DOM_client = 0b01,      // client accesses check against permission bits in tlb
    DOM_reserved = 0b10,      // client accesses check against permission bits in tlb
    DOM_manager = 0b11,      // TLB access bits are ignored.
};

// arm-vm-helpers: print <f>
void fld_print(fld_t *f);

// make a default fld: tag must be <0b10>
fld_t fld_mk(void);

/***********************************************************************
 * utility routines [in arm-vm-helpers.c]
 */

// hash [data, data+n) and print it with <msg>
void hash_print(const char *msg, const void *data, unsigned n);

// vm-helpers.c: one-time sanity check of the offsets in the structures above.
void check_vm_structs(void);

// domain access control  b4-42
void domain_acl_print(void);

//#include "mmu.h"

#endif
