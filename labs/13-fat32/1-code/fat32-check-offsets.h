#ifndef __FAT32_CHECK_OFFSETS_H__
#define __FAT32_CHECK_OFFSETS_H__
// engler, cs140e: sanity checking for structure size/offsets of the main 
// fat32 file system structures.  it.  would.  totally.  suck.  if one of these
// was slightly off.  so we assert the hell out of it.

#include "libc/helper-macros.h"

// master boot record
check_off_static(struct mbr, code, 0, 446);
check_off_static(struct mbr, part_tab1, 446, 16);
check_off_static(struct mbr, part_tab2, 462, 16);
check_off_static(struct mbr, part_tab3, 478, 16);
check_off_static(struct mbr, part_tab4, 494, 16);
check_off_static(struct mbr, sigval, 510, 2);

// file system info
check_off_static(struct fsinfo, sig1, 0, 4);
check_off_static(struct fsinfo, _reserved0, 4, 480);
check_off_static(struct fsinfo, sig2, 484, 4);
check_off_static(struct fsinfo, free_cluster_count, 488, 4);
check_off_static(struct fsinfo, next_free_cluster, 492, 4);
check_off_static(struct fsinfo, _reserved1, 496, 12);
check_off_static(struct fsinfo, sig3, 508, 4);

// the big fat32 boot sector struct
check_off_static(fat32_boot_sec_t, asm_code, 0, 3);
check_off_static(fat32_boot_sec_t, oem, 3, 8);
check_off_static(fat32_boot_sec_t, bytes_per_sec, 11, 2);
check_off_static(fat32_boot_sec_t, sec_per_cluster, 13, 1);
check_off_static(fat32_boot_sec_t, reserved_area_nsec, 14, 2);
check_off_static(fat32_boot_sec_t, nfats, 16, 1);
check_off_static(fat32_boot_sec_t, max_files, 17, 2);
check_off_static(fat32_boot_sec_t, fs_nsec, 19, 2);
check_off_static(fat32_boot_sec_t, media_type, 21, 1);
check_off_static(fat32_boot_sec_t, zero, 22, 2);
check_off_static(fat32_boot_sec_t, sec_per_track, 24, 2);
check_off_static(fat32_boot_sec_t, n_heads, 26, 2);
check_off_static(fat32_boot_sec_t, hidden_secs, 28, 4);
check_off_static(fat32_boot_sec_t, nsec_in_fs, 32, 4);
check_off_static(fat32_boot_sec_t, nsec_per_fat, 36, 4);
check_off_static(fat32_boot_sec_t, mirror_flags, 40, 2);
check_off_static(fat32_boot_sec_t, version, 42, 2);
check_off_static(fat32_boot_sec_t, first_cluster, 44, 4);
check_off_static(fat32_boot_sec_t, info_sec_num, 48, 2);
check_off_static(fat32_boot_sec_t, backup_boot_loc, 50, 2);
check_off_static(fat32_boot_sec_t, reserved, 52, 12);
check_off_static(fat32_boot_sec_t, logical_drive_num, 64, 1);
check_off_static(fat32_boot_sec_t, reserved1, 65, 1);
check_off_static(fat32_boot_sec_t, extended_sig, 66, 1);
check_off_static(fat32_boot_sec_t, serial_num, 67, 4);
check_off_static(fat32_boot_sec_t, volume_label, 71, 11);
check_off_static(fat32_boot_sec_t, fs_type, 82, 8);
check_off_static(fat32_boot_sec_t, ignore, 90, 420);
check_off_static(fat32_boot_sec_t, sig, 510, 2);


// fat32_directory structure
check_off_static(fat32_dir_t, filename, 0, 11);
check_off_static(fat32_dir_t, attr, 11, 1);
check_off_static(fat32_dir_t, reserved0, 12, 1);
check_off_static(fat32_dir_t, ctime_tenths, 13, 1);
check_off_static(fat32_dir_t, ctime, 14, 2);
check_off_static(fat32_dir_t, create_date, 16, 2);
check_off_static(fat32_dir_t, access_date, 18, 2);
check_off_static(fat32_dir_t, hi_start, 20, 2);
check_off_static(fat32_dir_t, mod_time, 22, 2);
check_off_static(fat32_dir_t, mod_date, 24, 2);
check_off_static(fat32_dir_t, lo_start, 26, 2);
check_off_static(fat32_dir_t, file_nbytes, 28, 4);

// lfn dir
check_off_static(lfn_dir_t, seqno, 0, 1);
check_off_static(lfn_dir_t, name1_5, 1, 10);
check_off_static(lfn_dir_t, attr, 11, 1);
check_off_static(lfn_dir_t, reserved, 12, 1);
check_off_static(lfn_dir_t, cksum, 13, 1);
check_off_static(lfn_dir_t, name6_11, 14, 12);
check_off_static(lfn_dir_t, reserved1, 26, 2);
check_off_static(lfn_dir_t, name12_13, 28, 4);

#endif
