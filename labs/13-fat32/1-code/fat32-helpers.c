#include "rpi.h"
#include "fat32.h"

/******************************************************************************
 * boot record helpers.
 */
#define is_pow2(x) (((x) & -(x)) == (x))

#include "helper-macros.h"

static int is_fat32(int t) { return t == 0xb; }

void fat32_volume_id_check(fat32_boot_sec_t *b) {
    // has to be a multiple of 512 or the sd.c driver won't really work.
    // currently assume 512
    assert(b->bytes_per_sec == 512);
    assert(b->nfats == 2);
    assert(b->sig == 0xAA55);

    assert(is_pow2(b->sec_per_cluster));
    unsigned n = b->bytes_per_sec;
    assert(n == 512 || n == 1024 || n == 2048 || n == 4096);
    assert(!b->max_files);
    // 0 if size does not fit in 2bytes, which should always be true.
    assert(b->fs_nsec == 0);
    // removable disk.
    // assert(b->media_type == 0xf0);
    assert(b->zero == 0);
    assert(b->nsec_in_fs != 0);

    // usually these apparently.
    assert(b->info_sec_num == 1);
    assert(b->backup_boot_loc == 6);
    assert(b->extended_sig == 0x29);
}

void fat32_volume_id_print(const char *msg, fat32_boot_sec_t *b) {
    printk("%s:\n", msg);
    char s[9];
    memcpy(s, b->oem, 8);
    s[8] = 0;
    printk("\toem               = <%s>\n", s);
    printk("\tbytes_per_sec     = %d\n", b->bytes_per_sec);
    printk("\tsec_per_cluster   = %d\n", b->sec_per_cluster);
    printk("\treserved size     = %d\n", b->reserved_area_nsec);
    printk("\tnfats             = %d\n", b->nfats);
    printk("\tmax_files         = %d\n", b->max_files);
    printk("\tfs n sectors      = %d\n", b->fs_nsec);
    printk("\tmedia type        = %x\n", b->media_type);
    printk("\tsec per track     = %d\n", b->sec_per_track);
    printk("\tn heads           = %d\n", b->n_heads);
    printk("\tn hidden secs     = %d\n", b->hidden_secs);
    printk("\tn nsec in FS      = %d\n", b->nsec_in_fs);
    printk("\tn nsec per fat    = %d\n", b->nsec_per_fat);
    printk("\tn mirror flags    = %b\n", b->mirror_flags);
    printk("\tn version         = %d\n", b->version);
    printk("\tn first_cluster   = %d\n", b->first_cluster);
    printk("\tn info_sec_num    = %d\n", b->info_sec_num);
    printk("\tn back_boot_loc   = %d\n", b->backup_boot_loc);
    printk("\tn logical_drive_num= %d\n", b->logical_drive_num);
    printk("\tn extended sig    = %x\n", b->extended_sig);
    printk("\tn serial_num      = %x\n", b->serial_num);
    printk("\tn volume label    = <%s>\n", b->volume_label);
    printk("\tn fs_type         = <%s>\n", b->fs_type);
    printk("\tn sig             = %x\n", b->sig);
    fat32_volume_id_check(b);
}

/****************************************************************************************
 * fsinfo helpers.
 */
void fat32_fsinfo_print(const char *msg, struct fsinfo *f) {
    printk("%s:\n", msg);
    printk("\tsig1              = %x\n", f->sig1);
    printk("\tsig2              = %x\n", f->sig2);
    printk("\tsig3              = %x\n", f->sig3);
    printk("\tfree cluster cnt  = %d\n", f->free_cluster_count);
    printk("\tnext free cluster = %x\n", f->next_free_cluster);
}

void fat32_fsinfo_check(struct fsinfo *info) {
    assert(info->sig1 ==  0x41615252);
    assert(info->sig2 ==  0x61417272);
    assert(info->sig3 ==  0xaa550000);
}


/******************************************************************************
 * FAT table helpers.
 */
const char * fat32_fat_entry_type_str(uint32_t x) {
    switch(x) {
    case FREE_CLUSTER:      return "FREE_CLUSTER";
    case RESERVED_CLUSTER:  return "RESERVED_CLUSTER";
    case BAD_CLUSTER:       return "BAD_CLUSTER";
    case LAST_CLUSTER:      return "LAST_CLUSTER";
    case USED_CLUSTER:      return "USED_CLUSTER";
    default: panic("bad value: %x\n", x);
    }
}

int fat32_fat_entry_type(uint32_t x) {
    // eliminate upper 4 bits: error to not do this.
    x = (x << 4) >> 4;
    switch(x) {
    case FREE_CLUSTER: 
    case RESERVED_CLUSTER:
    case BAD_CLUSTER:
        return x;
    }
    if(x >= 0x2 && x <= 0xFFFFFEF)
        return USED_CLUSTER;
    if(x >= 0xFFFFFF0 && x <= 0xFFFFFF6)
        panic("reserved value: %x\n", x);
    if(x >=  0xFFFFFF8  && x <= 0xFFFFFFF)
        return LAST_CLUSTER;
    panic("impossible type value: %x\n", x);
}

/****************************************************************************************
 * directory helpers.
 */

int fat32_dirent_free(fat32_dir_t *d) {
    uint8_t x = d->filename[0];

    if(d->attr == FAT32_LONG_FILE_NAME)
        return fat32_dirent_is_deleted_lfn(d);
    return x == 0 || x == 0xe5;
}

const char * fat32_dir_attr_str(int attr) {
    if(attr == FAT32_LONG_FILE_NAME)
        return " LONG FILE NAME";

    static char buf[128];
    buf[0] = 0;
    if(fat32_is_attr(attr, FAT32_RO)) {
        strcat(buf, "R/O");
        attr &= ~FAT32_RO;
    }
    if(fat32_is_attr(attr, FAT32_HIDDEN)) {
        strcat(buf, " HIDDEN");
        attr &= ~FAT32_HIDDEN;
    }

    switch(attr) {
    case FAT32_SYSTEM_FILE:     strcat(buf, " SYSTEM FILE"); break;
    case FAT32_VOLUME_LABEL:    strcat(buf, " VOLUME LABEL"); break;
    case FAT32_DIR:             strcat(buf, " DIR"); break;
    case FAT32_ARCHIVE:         strcat(buf, " ARCHIVE"); break;
    default: panic("unhandled attr=%x\n", attr);
    }
    return buf;
}

// trim spaces from the first 8.
// not re-entrant, gross.
static const char *to_8dot3(const char p[11]) {
    static char s[14];
    const char *suffix = &p[8];

    // find last non-space
    memcpy(s, p, 8);
    int i = 7; 
    for(; i >= 0; i--) {
        if(s[i] != ' ') {
            i++;
            break;
        }
    }
    s[i++] = '.';
    // probably need to handle spaces here too.
    for(int j = 0; j < 3; j++)
        s[i++] = suffix[j];
    s[i++] = 0;
    return s;
}


// UGH. fix all this conversion stuff: gross!!
void fat32_dirent_name(fat32_dir_t *d, char *name, unsigned n) {
    strcpy(name, to_8dot3(d->filename));
}

// not re-entrant, gross.
static const char *add_nul(const char filename[11]) {
    static char s[14];
    memcpy(s, filename, 11);
    s[11] = 0;
    return s;
}

void fat32_dirent_print_helper(fat32_dir_t *d) {
    if(fat32_dirent_free(d))  {
        printk("\tdirent is not allocated\n");
        return;
    }
    if(d->attr == FAT32_LONG_FILE_NAME) {
        printk("\tdirent is an LFN\n");
        return;
    }
    if(fat32_is_attr(d->attr, FAT32_ARCHIVE)) {
        printk("[ARCHIVE]: asssuming short part of LFN:");
    } else if(!fat32_is_attr(d->attr, FAT32_DIR)) {
        printk("need to handle attr %x (%s)\n", d->attr, fat32_dir_attr_str(d->attr));
        if(fat32_is_attr(d->attr, FAT32_ARCHIVE))
            return;
    }
    printk("\n");
    printk("\tfilename      = raw=<%s> 8.3=<%s>\n", add_nul(d->filename),to_8dot3(d->filename));
    printk("\tbyte version  = {");
    for(int i = 0; i < sizeof d->filename; i++) {
        if(i==8) {
            printk("\n"); printk("\t\t\t\t");
        }
        printk("'%c'/%x,", d->filename[i],d->filename[i]);
    }
    printk("}\n");
        
    printk("\tattr         = %x ", d->attr);
    if(d->attr != FAT32_LONG_FILE_NAME) {
        if(d->attr&FAT32_RO)
            printk(" [Read-only]\n");
        if(d->attr&FAT32_HIDDEN)
            printk(" [HIDDEN]\n");
        if(d->attr&FAT32_SYSTEM_FILE)
            printk(" [SYSTEM FILE: don't move]\n");
        printk("\n");
    }
#if 0
    // clutters output without much value (for lab 13 at least)
    printk("\tctime_tenths = %d\n", d->ctime_tenths);
    printk("\tctime         = %d\n", d->ctime);
    printk("\tcreate_date   = %d\n", d->create_date);
    printk("\taccess_date   = %d\n", d->access_date);
    printk("\tmod_time      = %d\n", d->mod_time);
    printk("\tmod_date      = %d\n", d->mod_date);
#endif
    printk("\thi_start      = %x\n", d->hi_start);
    printk("\tlo_start      = %d\n", d->lo_start);
    printk("\tfile_nbytes   = %d\n", d->file_nbytes);
}

void fat32_dirent_print(const char *msg, fat32_dir_t *d) {
    printk("%s: ", msg);
    fat32_dirent_print_helper(d);
}

int fat32_dir_lookup(const char *name, fat32_dir_t *dirs, unsigned n) {
    for(int i = 0; i < n; i++) {
        fat32_dir_t *d = dirs+i;
        if(fat32_dirent_free(d) || fat32_dirent_is_lfn(d))
            continue;
        if(memcmp(name, d->filename, sizeof d->filename) == 0)
            return i;
    }
    return -1;
}


/****************************************************************************************
 * general purpose utilities.
 */

// print [p, p+n) as a string: use for ascii filled files.
void print_as_string(const char *msg, uint8_t *p, int n) {
    printk("%s\n", msg);
    for(int i = 0; i < n; i++) {
        char c = p[i];
        printk("%c", c);
    }
    printk("\n");
}

void print_bytes(const char *msg, uint8_t *p, int n) {
    printk("%s\n", msg);
    for(int i = 0; i < n; i++) {
        if(i % 16 == 0)
            printk("\n\t");
        printk("%x, ", p[i]);
    }
    printk("\n");
}
void print_words(const char *msg, uint32_t *p, int n) {
    printk("%s\n", msg);
    for(int i = 0; i < n; i++) {
        if(i % 16 == 0)
            printk("\n\t");
        printk("0x%x, ", p[i]);
    }
    printk("\n");
}
