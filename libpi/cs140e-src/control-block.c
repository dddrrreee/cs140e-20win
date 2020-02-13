// trivial routines to access control block.
#include "rpi.h"
#include "control-block.h"

control_blk_t *cb_new(void) {
    control_blk_t *c = CTRL_BLK_ADDR;
    memset(c, 0, sizeof c);
    c->magic = CTRL_BLK_MAGIC;
    return c;
}

control_blk_t *cb_get_block(void) {
    control_blk_t *c = CTRL_BLK_ADDR;
    if(c->magic != CTRL_BLK_MAGIC)
        return 0;
    return c;
}

