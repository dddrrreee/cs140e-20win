/****************************************************************************************
 * engler, cs140e: long file name helpers.  based on wikipedia's writeup.
 */
#include "rpi.h"
#include "fat32.h"

int is_attr(uint32_t x, uint32_t flag) {
    if(x == FAT32_LONG_FILE_NAME)
        return x == flag;
    return (x & flag) == flag;
}
uint8_t lfn_checksum(const uint8_t *pFCBName) {
   uint8_t sum = 0;

   for (int i = 11; i; i--)
      sum = ((sum & 1) << 7) + (sum >> 1) + *pFCBName++;
   return sum;
}

int lfn_is_last(uint8_t seqno) {
    return (seqno & (1<<6)) != 0;
}
int lfn_is_first(uint8_t seqno) {
    // remove is-last bit
    seqno &= ~(1<<6);
    return seqno == 1;
}
int lfn_is_deleted(uint8_t seqno) {
    return (seqno & 0xe5) == 0xe5;
}
int fat32_dirent_is_deleted_lfn(fat32_dir_t *d) {
    assert(d->attr == FAT32_LONG_FILE_NAME);
    lfn_dir_t *l = (void*)d;
    return lfn_is_deleted(l->seqno);
}

void lfn_print_ent(lfn_dir_t *l, uint8_t cksum) {
    uint8_t n = l->seqno;
    printk("\tseqno = %x, first=%d, last=%d, deleted=%d\n", 
            n, lfn_is_first(n), lfn_is_last(n), lfn_is_deleted(n));
 
    uint8_t buf[27];
    memcpy(&buf[0],  l->name1_5,  10);
    memcpy(&buf[10], l->name6_11, 12);
    memcpy(&buf[22], l->name12_13, 4);
    buf[26] = 0;
    
    for(int i = 0; i < 26; i += 2) {
        if(buf[i] == 0 && buf[i+1] == 0)
            break;
        printk("lfn[%d] = '%c' = %x\n", i, buf[i], buf[i]);
    }
    printk("\tcksum=%x (expected=%x)\n", l->cksum, cksum);
}

#include "unicode-utf8.h"

static inline int lfn_terminator(uint8_t *x) {
    if(x[0] == 0 && x[1] == 0)
        return 1;
    if(x[0] == 0xff && x[1] == 0xff)
        return 1;
    return 0;
}

static char *utf8_append(char *buf, uint8_t *unicode, unsigned nbytes) {
    assert(nbytes%2 == 0);
    for(int i = 0; i < nbytes; i+=2) {
        if(lfn_terminator(unicode+i))
            break;
        char *utf8 = to_utf8(unicode[i] | (unicode[i+1]<<8));
        strcat(buf, utf8);
        buf += strlen(utf8);
    }
    return buf;
}

char *lfn_get_name(lfn_dir_t *s, int cnt) {
    static char filename[512];
    filename[0] = 0;
    char *buf = filename;
    for(int i = cnt-1; i >= 0; i--) {
        lfn_dir_t *l = s+i;
        assert(l->attr == FAT32_LONG_FILE_NAME);
        buf = utf8_append(buf, l->name1_5, sizeof l->name1_5);
        buf = utf8_append(buf, l->name6_11, sizeof l->name6_11);
        buf = utf8_append(buf, l->name12_13, sizeof l->name12_13);
    }
    *buf = 0;
    return filename;
}

void lfn_print(lfn_dir_t *s, int cnt, uint8_t cksum, int print_ent_p) {
    assert(cnt >= 1);

    // weird if larger.
    demand(cnt > 0 && cnt < 3, weird for cs140e if larger!);
    for(int i = 0;  i < cnt; i++) {
        assert(!lfn_is_deleted((s+i)->seqno)); 
        if(print_ent_p) {
            printk("lfn[%d]=\n", i);
            lfn_print_ent(s+i, cksum);
        }
        assert(s[i].cksum == cksum);
    }
    assert(lfn_is_last(s->seqno));
    assert(lfn_is_first((s+cnt-1)->seqno));
}

// reconstruct file name, return pointer to the dir-entry.
fat32_dir_t *fat32_dir_filename(char *name, fat32_dir_t *d, fat32_dir_t *end) {
    assert(!fat32_dirent_free(d));

    uint32_t x = d->attr;

    if(x == FAT32_LONG_FILE_NAME) {
        int cnt = 1, n = end-d;
        for(; cnt < n; cnt++)
            if(d[cnt].attr != FAT32_LONG_FILE_NAME)
                break;
        assert(cnt < n);
        assert(is_attr(d[cnt].attr, FAT32_DIR) || is_attr(d[cnt].attr, FAT32_ARCHIVE));
        strcpy(name, lfn_get_name((void*)d, cnt));
        return d+cnt;
    } else {
        assert(is_attr(x, FAT32_DIR) 
            || is_attr(x, FAT32_ARCHIVE) 
            || is_attr(x, FAT32_VOLUME_LABEL));
        int i=0,j=0, lower_case_p = 0;

        // macos?
        if(d->reserved0 == 0x18)
            lower_case_p = 1;

        for(; i < 8; i++) {
            char c = d->filename[i];
            // skip spaces.
            if(c == ' ')
                continue;
            if(lower_case_p && c >= 'A' && c <= 'Z')
                c += 32;
            name[j++] = c;
        }
        // implied b/n bytes 7 and 8
        name[j++] = '.';
        for(; i < 11; i++) {
            char c = d->filename[i];
            if(lower_case_p && c >= 'A' && c <= 'Z')
                c += 32;
            name[j++] = c;
        }
        name[j] =0;
        return d;
    } 
}

// returns number of entries to advance forward.
int fat32_lfn_print(const char *msg, fat32_dir_t *d, int left) {

    if(d->attr != FAT32_LONG_FILE_NAME) {
        fat32_dirent_print(msg, d);
        return 1;
    }
    printk("%s: ", msg);

    int cnt;
    for(cnt = 0; cnt < left; cnt++)
        if(d[cnt].attr != FAT32_LONG_FILE_NAME)
                break;

    assert(cnt < left);
    assert(is_attr(d[cnt].attr, FAT32_DIR) || is_attr(d[cnt].attr, FAT32_ARCHIVE));
    printk("\n");
    lfn_print((void*)d, cnt, lfn_checksum(d[cnt].filename),0);

    char *name = lfn_get_name((void*)d, cnt);
    printk("\treconstructed filename = <%s>\n\t", name);

    d = &d[cnt];
    cnt++;

    // print the rest.
    fat32_dirent_print_helper(d);
    return cnt;
}
