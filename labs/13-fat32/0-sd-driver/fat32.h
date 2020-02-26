#ifndef __RPI_FAT32_H__
#define __RPI_FAT32_H__
/* 
 * engler, cs140e: useful structures for dealing with fat32 and sd cards.
 * the definitive source is the microsoft specification (which actually
 * isn't too bad and is fairly short).   but the links below give even
 * more concise summaries.
 *
 *      ****** DO NOT MODIFY THIS FILE ********
 *      ****** DO NOT MODIFY THIS FILE ********
 *      ****** DO NOT MODIFY THIS FILE ********
 *      ****** DO NOT MODIFY THIS FILE ********
 *      ****** DO NOT MODIFY THIS FILE ********
 */

/*
  Copied from from: 
   https://www.win.tue.nl/~aeb/linux/fs/fat/fat-1.html

  layout:
    First the boot sector (at relative address 0), and possibly other
    stuff. Together these are the Reserved Sectors. Usually the boot
    sector is the only reserved sector.

    Then the FATs (following the reserved sectors; the number of reserved
    sectors is given in the boot sector, bytes 14-15; the length of a
    sector is found in the boot sector, bytes 11-12).

    Then the Root Directory (following the FATs; the number of FATs is
    given in the boot sector, byte 16; each FAT has a number of sectors
    given in the boot sector, bytes 22-23).

    Finally the Data Area (following the root directory; the number of
    root directory entries is given in the boot sector, bytes 17-18,
    and each directory entry takes 32 bytes; space is rounded up to
    entire sectors).
*/
struct partition_entry {
    uint32_t bootable_p:8,
             chs_start:24,
             part_type:8,
             chs_end:24;
    uint32_t lba_start;
    uint32_t nsec;
} __attribute__ ((packed));
_Static_assert (sizeof(struct partition_entry) == 16, "partition_entry: size wrong");


/*
Byte Range  Description Essential
0-445   Boot Code   No
446-461 Partition Table Entry #1    Yes
462-477 Partition Table Entry #2    Yes
478-493 Partition Table Entry #3    Yes
494-509 Partition Table Entry #4    Yes
510-511 Signature value (0xAA55)    Yes
*/
struct mbr {
    uint8_t     code[446],
                part_tab1[16],
                part_tab2[16],
                part_tab3[16],
                part_tab4[16];
    uint16_t sigval;
};
_Static_assert(sizeof(struct mbr) == 512, "mbr size wrong");

/*
Bytes   Content
0-3     0x41615252 - the FSInfo signature
4-483   Reserved
484-487 0x61417272 - a second FSInfo signature
488-491 Free cluster count or 0xffffffff (may be incorrect)
492-495 Next free cluster or 0xffffffff (hint only)
496-507 Reserved
508-511 0xaa550000 - sector signature
*/
struct fsinfo {
    uint32_t sig1;
    uint8_t _reserved0[480];
    uint32_t sig2;
    uint32_t free_cluster_count;
    uint32_t next_free_cluster;
    uint8_t _reserved1[12];
    uint32_t sig3;
};
_Static_assert(sizeof(struct fsinfo) == 512, "fsinfo size wrong");

// https://www.win.tue.nl/~aeb/linux/fs/fat/fat-1.html
typedef struct {
    uint8_t asm_code[3];            // 0-2  Assembly code instructions to jump to boot 
                                    //      code (mandatory in bootable partition)
    uint8_t oem[8];                 // 3-10    OEM name in ASCII
    uint16_t bytes_per_sec;         // 11-12   Bytes per sector (512, 1024, 2048, or 4096)
    uint8_t sec_per_cluster;        // 13  Sectors per cluster (Must be a power of 2 and 
                                    //      cluster size must be <=32 KB)
    uint16_t reserved_area_nsec;    // 14-15   Size of reserved area, in sectors
    uint8_t nfats;                  // 16  Number of FATs (usually 2)
    uint16_t max_files;             // 17-18   Maximum number of files in the root 
                                    //      directory (FAT12/16; 0 for FAT32)
    uint16_t fs_nsec;               // 19-20   Number of sectors in the file system; if 
                                    //      2 B is not large enough, set to 0 and use 4 B 
                                    //      value in bytes 32-35 below
    uint8_t media_type;             // 21  Media type (0xf0=removable disk, 0xf8=fixed disk)
    uint16_t zero;                  // 22-23 Size of each FAT, 0 for FAT32
    uint16_t sec_per_track;         // 24-25   Sectors per track in storage device

    uint16_t n_heads;               // 26-27   Number of heads in storage device
    uint32_t hidden_secs;           // 28-31   Number of hidden sectors (0)
    uint32_t nsec_in_fs;            // 32-35   Total number of sectors in the filesystem
    uint32_t nsec_per_fat;          // 36-39   Sectors per FAT
    uint16_t  mirror_flags;         // 40-41   Mirror flags
                                    //  Bits 0-3: number of active FAT (if bit 7 is 1)
                                    //  Bits 4-6: reserved
                                    //  Bit 7: one: single active FAT; zero: all FATs are 
                                    //              updated at runtime
                                    //  Bits 8-15: reserved
    uint16_t  version;              // 42-43   Filesystem version
    uint32_t  first_cluster;        // 44-47   First cluster of root directory (usually 2)
    uint16_t  info_sec_num;         // 48-49   Filesystem information sector number in 
                                    //         FAT32 reserved area (usually 1)
    uint16_t backup_boot_loc;       // 50-51   Backup boot sector location or 0 or 0xffff 
                                    //         if none (usually 6)
    uint8_t reserved[12];           // 52-63   Reserved
    uint8_t logical_drive_num;      // 64      Logical Drive Number (for use with 
                                    //          INT 13, e.g. 0 or 0x80)
    uint8_t reserved1;              // 65      Reserved - used to be Current Head 
                                    //      (used by Windows NT)
    uint8_t extended_sig;           // 66      Extended signature (0x29)
                                    // Indicates that the three following fields are present.
    uint32_t serial_num;            // 67-70   Serial number of partition
    uint8_t volume_label[11];       // 71-81   Volume label
    uint8_t fs_type[8];             // 82-89   Filesystem type ("FAT32   ")
    uint8_t ignore[420];                // 90 -- 509
    uint16_t sig;                   // 510-511 = 0xAA55
} __attribute__ ((packed)) fat32_boot_sec_t;

_Static_assert(sizeof(fat32_boot_sec_t) == 512, "volume_id size is wrong");


// directory attribute types.
enum {
    FAT32_RO = 0x01,
    FAT32_HIDDEN = 0x02,
    FAT32_SYSTEM_FILE = 0x04,
    FAT32_VOLUME_LABEL = 0x08,
    FAT32_LONG_FILE_NAME = 0x0f,
    FAT32_DIR = 0x10,
    FAT32_ARCHIVE = 0x20
};


// additional comments
//  http://www.c-jump.com/CIS24/Slides/FileSysDataStructs/FileSysDataStructs.html
typedef struct {
    // if filename[0] == 0, then not allocated.
    // 2-11 char of file name.  the "." is implied b/n bytes7 and 8
    uint8_t filename[11];   // 0-10    File name (8 bytes) with extension (3 bytes)
    uint8_t attr;           // 11      Attribute - a bitvector. 
                            //      Bit 0: read only. 
                            //      Bit 1: hidden.
                            //      Bit 2: system file. 
                            //      Bit 3: volume label. 
                            //      Bit 4: subdirectory.
                            //      Bit 5: archive. 
                            //      Bits 6-7: unused.
    uint8_t reserved0;      // 12   Reserved (see below)
    // in tenths of second
    uint8_t ctime_tenths;   // 13 file creation time (tenths of sec)
    uint16_t ctime;         // 14-15 create time hours,min,sec
    uint16_t create_date;   // 16-17 create date
    uint16_t access_date;   // 18-19 create date
    uint16_t hi_start;      // 20-21 high two bytes of first cluster
    uint16_t mod_time;      // 22-23   Time (5/6/5 bits, for hour/minutes/doubleseconds)
    uint16_t mod_date;      // 24-25   Date (7/4/5 bits, for year-since-1980/month/day)
    uint16_t lo_start;      // 26-27   low order 2 bytes of first cluster
    // 0 for directories
    uint32_t file_nbytes;   // 28-31   Filesize in bytes
} __attribute__ ((packed)) fat32_dir_t;


_Static_assert(sizeof(fat32_dir_t) == 32, "fat32_dir size is wrong");

#define NDIR_PER_SEC (512/32) 
_Static_assert(sizeof(fat32_dir_t) * NDIR_PER_SEC == 512, "NDIR_PER_SEC wrong");

typedef struct {
    uint8_t seqno;          // 0-0 Sequence number (ORed with 0x40) and allocation 
                            // status (0xe5 if unallocated)   
    uint8_t name1_5[10];   // 1-10    File name characters 1-5 (Unicode) 
    uint8_t attr;           // 11-11   File attributes (always 0x0f)
    uint8_t reserved;       // 12-12   Reserved   
    uint8_t cksum;          // 13-13   Checksum  
    uint8_t name6_11[12];   // 14-25   File name characters 6-11 (Unicode) 
    uint16_t reserved1;     // 26-27   Reserved   
    uint8_t name12_13[4];   // 28-31   File name characters 12-13 (Unicode) 
} __attribute__ ((packed)) lfn_dir_t;
_Static_assert(sizeof(lfn_dir_t) == 32, "lfn_dir_ent size is wrong");

/*
    https://www.forensicswiki.org/wiki/FAT
    0x?0000000 (Free Cluster)
    0x?0000001 (Reserved Cluster)
    0x?0000002 - 0x?FFFFFEF (Used cluster; value points to next cluster)
    0x?FFFFFF0 - 0x?FFFFFF6 (Reserved values)
    0x?FFFFFF7 (Bad cluster)
    0x?FFFFFF8 - 0x?FFFFFFF (Last cluster in file)

    Note: FAT32 uses only 28 of 32 possible bits, the upper 4 bits should
    be left alone. Typically these bits are zero, and are represented
    above by a question mark (?).

*/
enum {
    FREE_CLUSTER = 0,
    RESERVED_CLUSTER = 0x1,
    BAD_CLUSTER = 0xFFFFFF7,
    LAST_CLUSTER,
    USED_CLUSTER,
};

// is this a master boot record?
void mbr_check(struct mbr *mbr);

static inline int mbr_part_is_fat32(int t) { return t == 0xb || t == 0xc; }

// print out the partition <p>
void mbr_partition_print(const char *msg, struct partition_entry *p);

// is partition empty?
int mbr_partition_empty(uint8_t *part);

#endif
