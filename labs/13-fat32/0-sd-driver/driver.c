/*
 * engler, cs140e: simple test driver.  You should read in an
 * master boot record (MBR) that makes sense.   extra credit if you 
 * get multiple drivers and check their results against each other.
 */
#include "rpi.h"
#include "fat32.h"
#include "bzt-sd.h"

int pi_sd_read(void *data, uint32_t lba, uint32_t nsec) {
    int res;
    if((res = sd_readblock(lba, data, nsec)) != 512 * nsec)
        panic("could not read from sd card: result = %d\n", res);
    return 1;
}
int pi_sd_init(void) {
    if(sd_init() != SD_OK)
        panic("sd_init failed\n");
    return 1;
}

// allocate <nsec> worth of space, read in from SD card, return pointer.
// your kmalloc better work!
void *sec_read(uint32_t lba, uint32_t nsec) {
    uint8_t *data = kmalloc(nsec * 512);
    if(!pi_sd_read(data, lba, nsec))
        panic("could not read from sd card\n");
    return data;
}

void notmain(void) {
    uart_init();

    printk("about to init\n");
    pi_sd_init();
    printk("done init\n");

    printk("about to read block 0\n");
    struct mbr *mbr = sec_read(0, 1);
    printk("read block 0\n");

    mbr_check(mbr);

    // get the first partition and print it.
    struct partition_entry p;
    memcpy(&p, mbr->part_tab1, sizeof p);
    mbr_partition_print("partition 1", &p);
    assert(mbr_part_is_fat32(p.part_type));

    // verify only the first partition is non-empty.
    // this could fail if you were fancy and re-partitioned your
    // sd card.  (we should actually do so so that we can make a
    // custom file system).
    assert(!mbr_partition_empty((uint8_t*)mbr->part_tab1));
    assert(mbr_partition_empty((uint8_t*)mbr->part_tab2));
    assert(mbr_partition_empty((uint8_t*)mbr->part_tab3));
    assert(mbr_partition_empty((uint8_t*)mbr->part_tab4));

    clean_reboot();
}
