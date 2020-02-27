#include "rpi.h"
#include "fat32.h"

// you should put this in external-code
#include "crc.h"
#include "bzt-sd.h"

static int trace_p = 1;

int pi_sd_read(void *data, uint32_t lba, uint32_t nsec) {
    int res;
    if((res = sd_readblock(lba, data, nsec)) != 512 * nsec)
        panic("could not read from sd card: result = %d\n", res);

    if(trace_p)
        pi_trace("sd_read: lba=<%x>, cksum=%x\n", lba, our_crc32(data,nsec*512));
    return 1;
}

int pi_sd_init(void) {
    if(sd_init() != SD_OK)
        panic("sd_init failed\n");
    return 1;
}

// allocate <nsec> worth of space, read in from SD card, return pointer.
// your kmalloc better work!
void *pi_sec_read(uint32_t lba, uint32_t nsec) {
    uint8_t *data = kmalloc(nsec * 512);
    if(!pi_sd_read(data, lba, nsec))
        panic("could not read from sd card\n");
    return data;
}
