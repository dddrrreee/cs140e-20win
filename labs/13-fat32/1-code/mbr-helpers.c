/*
 * engler, cs140e: helpers for master boot record.  
 * do not modify this file!   will be pushing changes.
 */
#include "rpi.h"
#include "fat32.h"

void mbr_check(struct mbr *mbr) { 
    assert(mbr->sigval == 0xAA55);

    // not aligned so have to copy.
    struct partition_entry p;
    memcpy(&p, mbr->part_tab1, sizeof p);

    assert(mbr_part_is_fat32(p.part_type));

    // verify only the first partition is non-empty.
    // this could fail if you were fancy and re-partitioned your
    // sd card.  (we should actually do so so that we can make a
    // custom file system).
    assert(!mbr_partition_empty((uint8_t*)mbr->part_tab1));
    assert(mbr_partition_empty((uint8_t*)mbr->part_tab2));
    assert(mbr_partition_empty((uint8_t*)mbr->part_tab3));
    assert(mbr_partition_empty((uint8_t*)mbr->part_tab4));
}

mbr_partition_ent_t mbr_get_partition(struct mbr *mbr, unsigned partno) {
    assert(partno < 4);
    mbr_partition_ent_t p;

    uint8_t *e = (void*)mbr->part_tab1;
    e += sizeof p * partno;
    memcpy(&p, e, sizeof p);
    return p;
}

// partition in master boot record.
const char *mbr_part_str(int x) {
#   define T(v, type) case v : return _XSTRING(type) ;
    switch(x) {
    T(0x00,   Empty)
    T(0x01,   FAT12 - CHS)
    T(0x04,   FAT16 - 16-32 MB - CHS)
    T(0x05,   Microsoft Extended - CHS)
    T(0x06,   FAT16 - 32 MB-2 GB - CHS)
    T(0x07,   NTFS)
    T(0x0b,   FAT32 - CHS)
    T(0x0c,   FAT32 - LBA)
    T(0x0e,   FAT16 - 32 MB-2 GB - LBA)
    T(0x0f,   Microsoft Extended - LBA)
    T(0x11,   Hidden FAT12 - CHS)
    T(0x14,   Hidden FAT16 - 16-32 MB - CHS)
    T(0x16,   Hidden FAT16 - 32 MB-2 GB - CHS)
    T(0x1b,   Hidden FAT32 - CHS)
    T(0x1c,   Hidden FAT32 - LBA)
    T(0x1e,   Hidden FAT16 - 32 MB-2 GB - LBA)
    T(0x42,   Microsoft MBR - Dynamic Disk)
    T(0x82,   Solaris x86 or Linux swap?)
    T(0x83,   Linux)
    T(0x84,   Hibernation)
    T(0x85,   Linux Extended)
    T(0x86,   NTFS Volume Set)
    T(0x87,   NTFS Volume Set)
    T(0xa0,   Hibernation)
    T(0xa1,   Hibernation)
    T(0xa5,   FreeBSD)
    T(0xa6,   OpenBSD)
    T(0xa8,   Mac OSX)
    T(0xa9,   NetBSD)
    T(0xab,   Mac OSX Boot)
    T(0xb7,   BSDI)
    T(0xb8,   BSDI swap)
    T(0xee,   EFI GPT Disk)
    T(0xef,   EFI System Partition)
    T(0xfb,   Vmware File System)
    T(0xfc,   Vmware swap)
    default: panic("unknown type: %x\n", x);
    }
}

int mbr_partition_empty(uint8_t *part) {
    return memiszero(part,16);
}

void mbr_partition_print(const char *msg, struct partition_entry *p) {
    printk("%s:\n", msg);
    printk("\tbootable  = %s\n", p->bootable_p ? "true" : "false");
    printk("\tchs_start = %x\n", p->chs_start);
    printk("\tpart type = %x (%s)\n", p->part_type, mbr_part_str(p->part_type));
    printk("\tchs_end   = %x\n", p->chs_end);
    printk("\tlba_start = %x\n", p->lba_start);
    printk("\tnsector=   = %d (%dGB)\n", p->nsec, p->nsec / (2*1024*1024));
    assert(p->part_type == 0xb || p->part_type == 0xc);
}
