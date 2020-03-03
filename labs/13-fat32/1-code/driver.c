/*
 * engler, cs140e: simple fat32 read-only driver.
 */
#include "rpi.h"
#include "fat32.h"
#include "crc.h"
#include "pi-message.h"

#define roundup(x, n) (((x)+((n)-1))&(~((n)-1)))

/*
 * aggregate the FAT32 information.  refer to paul's writeup for
 * how to compute thes.   you can compute each using either:
 *    1. the "volume id" (our <fat32_boot_sec_t> struct in fat32.h)
 *    2. or the boot partition (should be partition 0 of the <mbr>
 *  
 * to access a cluster:
 *      cluster lba = cluster_begin + (cluster_number - 2) * sectors_per_cluster
 */
typedef struct fat32 {
    uint32_t    
        lba_start,                  // start of partition
        fat_begin_lba,              // start of partition + nreserved sectors
        cluster_begin_lba,          // above + (2 * nsec per fat)
        sectors_per_cluster,        
        root_dir_first_cluster,     // lba of first_cluster
        *fat,                       // pointer to in-memory copy of FAT
        n_fat;                      // number of entries in the FAT table.
} fat32_fs_t;

/*
  Field                      Microsoft's Name    Offset   Size        Value
  Bytes Per Sector           BPB_BytsPerSec      0x0B(11) 16 Bits     Always 512 Bytes
  Sectors Per Cluster        BPB_SecPerClus      0x0D(13) 8 Bits      1,2,4,8,16,32,64,128
  Number of Reserved Sectors BPB_RsvdSecCnt      0x0E(14) 16 Bits     Usually 0x20
  Number of FATs             BPB_NumFATs         0x10(16) 8 Bits      Always 2
  Sectors Per FAT            BPB_FATSz32         0x24(36) 32 Bits     Depends on disk size
  Root Directory 1st Cluster BPB_RootClus        0x2C(44) 32 Bits     Usually 0x00000002
  Signature                  (none)              0x1FE(510)16 Bits     Always 0xAA55

  create fat32 fs structure.  do sanity checking (some listed above).
*/
fat32_fs_t fat32_mk(uint32_t lba_start, fat32_boot_sec_t *b) {
    fat32_fs_t fs;

    // let me know if this is true on your system.
    unsigned char *r = (void*)&b->reserved_area_nsec;
    unsigned reserved = (r[1] << 8) | r[0];
    assert(reserved == b->reserved_area_nsec);

    fs.lba_start = lba_start;
    fs.fat_begin_lba = lba_start + b->reserved_area_nsec;
    printk("begin lba = %d\n", fs.fat_begin_lba);

	fs.cluster_begin_lba = fs.fat_begin_lba + 2 * b->nsec_per_fat;
	fs.sectors_per_cluster = b->sec_per_cluster;
	fs.root_dir_first_cluster = b->first_cluster;
	fs.n_fat = (b->nsec_per_fat * b->bytes_per_sec / 4); 

    /* 
     * read in the entire fat (one copy: worth reading in the second and
     * comparing).

     * The disk is divided into clusters. The number of sectors per
     * cluster is given in the boot sector byte 13. <sec_per_cluster>

     * The File Allocation Table has one entry per cluster. This entry
     * uses 12, 16 or 28 bits for FAT12, FAT16 and FAT32.
    */
    
	fs.fat = pi_sec_read(fs.fat_begin_lba, b->nsec_per_fat * b->nfats);	

    return fs;
}

// given cluster_number get lba
uint32_t cluster_to_lba(struct fat32 *f, uint32_t cluster_num) {
    assert(cluster_num >= 2);
	return f->cluster_begin_lba + (cluster_num - 2) * f->sectors_per_cluster;
}

/*
        fat_begin_lba,              // start of partition + nreserved sectors
        cluster_begin_lba,          // above + (2 * nsec per fat)
        sectors_per_cluster,        
        root_dir_first_cluster,     // lba of first_cluster
*/

fat32_dir_t *fat32_readin_root_dir(uint32_t *dir_n, fat32_fs_t *fs) {
    int type = fat32_fat_entry_type(fs->fat[2]);
    printk("fat[2] = %x, type=%s\n", fs->fat[2], fat32_fat_entry_type_str(type));
    printk("lba.start=%d\n", fs->lba_start);
    printk("cluster 2 to lba = %d\n", cluster_to_lba(fs, 2));

    // root directory is fixed size = 1 cluster.
    unsigned dir_lba = cluster_to_lba(fs, 2);       // fill this in
    uint32_t dir_nsecs = fs->sectors_per_cluster;     // fill this in

    printk("rood dir first cluster = %d\n", dir_lba);
    *dir_n = NDIR_PER_SEC * dir_nsecs;
    return pi_sec_read(dir_lba, dir_nsecs);
}

/**************************************************************************
 * trivial ramfs representation.
 */

// from the fuse lab.
//
// this way of creating a ramFS is not the right way to do things.
// we use expedient hacks so we can strip the things down for a 
// couple hour lab.
typedef struct {
    char *data;
    size_t n_alloc,    // total bytes allocated.
             n_data;     // how many bytes of data
} pi_file_t;

typedef struct {
    char name[16], raw_name[16];
    uint32_t cluster_id, is_dir_p, nbytes;
} dirent_t;

dirent_t fat32_dirent_convert(fat32_dir_t *d) {
    dirent_t e = {
        .cluster_id = fat32_cluster_id(d),
        .is_dir_p = fat32_is_dir(d),
        .nbytes = d->file_nbytes
    };
    // can compare this name
    memcpy(e.raw_name, d->filename, sizeof d->filename);
    // for printing.
    fat32_dirent_name(d,e.name,sizeof e.name);
    return e;
}

dirent_t *dirent_fat32_lookup(const char *raw_name, fat32_dir_t *dirs, unsigned n) {
    int i = fat32_dir_lookup(raw_name, dirs, n);
    if(i < 0)
        return 0;
    dirent_t *e = kmalloc(sizeof *e);
    *e = fat32_dirent_convert(dirs+i);
    return e;
}
void dirent_print(dirent_t *e) { 
    printk("%s\t\t->\tcluster id=%d, type=%s, nbytes=%d\n",
        e->name, e->cluster_id, e->is_dir_p?"dir":"file", e->nbytes);
}

#define END_CODE 0xFFFFFFF

// you may have to read in many clusters: this should be a loop.
// as a first cut (for config) try just reading one cluster.
pi_file_t fat32_read_file(fat32_fs_t *fs, dirent_t *d) {
    uint32_t id = d->cluster_id;

    pi_file_t f;

	f.n_data = d->nbytes;
	f.n_alloc = roundup(f.n_data, 512 * fs->sectors_per_cluster);
	f.data = kmalloc(f.n_alloc);
	
	memset(f.data, 0, f.n_alloc);
	char* file_ptr = f.data;

	uint32_t next_id = id;
	uint32_t* fat_walker = fs->fat + next_id;

	while(fat32_fat_entry_type(next_id) != LAST_CLUSTER) {
		printk("TRYING TO READ CLUSTER_ID: %d\n", next_id);
		pi_sd_read(file_ptr, cluster_to_lba(fs, next_id),
				   fs->sectors_per_cluster);
		next_id = (*fat_walker) & 0xFFFFFFF;
		fat_walker = fs->fat + next_id;
		file_ptr += 512 * fs->sectors_per_cluster; 
	}
	
	if(fat32_fat_entry_type(next_id) == LAST_CLUSTER) {
		printk("TRYING TO READ CLUSTER_ID: %d\n", next_id);
		pi_sd_read(file_ptr, cluster_to_lba(fs, next_id), fs->sectors_per_cluster);
	}

    // done.
    return f;
}

void notmain(void) {
    kmalloc_init();
	uart_init();

    //*********************************************************************
    // PART1

    // initialize the sd and load the mbr
    pi_sd_init();
    struct mbr *mbr = pi_sec_read(0, 1);
    mbr_check(mbr);

    // get the first partition and print it.
    mbr_partition_ent_t p = mbr_get_partition(mbr, 0);
    mbr_partition_print("partition 0", &p);

    //*********************************************************************
    // PART2

    /*
        https://www.pjrc.com/tech/8051/ide/fat32.html
        The first step to reading the FAT32 filesystem is the read its
        first sector, called the Volume ID. The Volume ID is read using
        the LBA Begin address found from the partition table. From this
        sector, you will extract information that tells you everything
        you need to know about the physical layout of the FAT32 filesystem

        NOTE: this is our <fat32_boot_sec_t>.   
    */
    printk("reading from %d\n", p.lba_start);
    fat32_boot_sec_t *b = pi_sec_read(p.lba_start, 1);
	
    // does a reasonably thorough set of checks.  please post if you figure out
    // more.
    fat32_volume_id_check(b);
    fat32_volume_id_print("boot sector", b);

    // this is what we add to <lba_start>
    assert(b->info_sec_num == 1);
    struct fsinfo *info = pi_sec_read(p.lba_start + b->info_sec_num, b->info_sec_num);

    fat32_fsinfo_check(info);
    fat32_fsinfo_print("info struct", info);

    //*********************************************************************
    // PART3

    // construct the fat file system structure
    fat32_fs_t fs = fat32_mk(p.lba_start, b);

    //*********************************************************************
    // PART4

    // read in the root directory.
    uint32_t n_dir;
    fat32_dir_t *dirs = fat32_readin_root_dir(&n_dir,&fs);

    printk("---------------------------------------------------------\n");
    printk("about to emit directories: n_dir = %d\n", n_dir);
    for(int i = 0; i < n_dir; i++) {
        fat32_dir_t *d = dirs+i;
        if(fat32_dirent_free(d)) {
            // printk("dir[%d]: is free\n", i);
        } else if(fat32_dirent_is_lfn(d)) {
            printk("dir[%d]: is an lfn\n", i);
        } else {
            printk("dir %d is not free!:", i);
            fat32_dirent_print("", dirs+i);
        }
    }

#if 0
    // if you want to print with lfns.
    for(int i = 0; i < n_dir; ) {
        if(fat32_dirent_free(dirs+i)) {
            i++;
        } else {
            printk("dir %d is not free!:", i);
            i += fat32_lfn_print("", dirs+i, n_dir - i);
        }
    }
#endif


    printk("---------------------------------------------------------\n");
    printk("about to dump out the clusters\n");
    for(int i = 0; i < n_dir; i++) {
        fat32_dir_t *d = dirs+i;
        if(fat32_dirent_free(d) || fat32_dirent_is_lfn(d)) 
            continue;
        dirent_t e = fat32_dirent_convert(d);
        printk("\t%d: ", i);
        dirent_print(&e);
    }

    //*********************************************************************
    // PART5

    // looking up raw name.
    char config[] = "CONFIG  TXT";
    dirent_t *e = dirent_fat32_lookup(config, dirs,n_dir);
    if(!e)
        panic("could not find <%s>??\n", config);
    dirent_print(e);

    printk("---------------------------------------------------------\n");
    pi_file_t f = fat32_read_file(&fs, e);
    print_as_string("going to concat config.txt", f.data, f.n_data);
    printk("---------------------------------------------------------\n");

    // cksum the bootcode
    char bootcode[] = "BOOTCODEBIN";
    e = dirent_fat32_lookup(bootcode, dirs,n_dir);
    f = fat32_read_file(&fs, e);
    printk("crc of bootcode (nbytes=%d) = %x\n", f.n_data, our_crc32(f.data,f.n_data));

    //*********************************************************************
    // PART6
    // read in and jump to hello-fixed.bin --- you probably should rename it 
    // something that is easy to get.
	char bin_file[] = "HELLO   BIN";
	e =  dirent_fat32_lookup(bin_file, dirs, n_dir);
    if(!e)
        panic("could not find <%s>??\n", bin_file);
    dirent_print(e);
    f = fat32_read_file(&fs, e);
	struct pi_bin_header h = *(struct pi_bin_header*) f.data;
	memcpy(h.addr, f.data, h.nbytes);
	BRANCHTO(h.addr + sizeof h); 
    clean_reboot();
}
