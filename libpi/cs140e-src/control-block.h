#ifndef __CONTROL_BLOCK_H__
#define __CONTROL_BLOCK_H__
// hopefully a free address we do not corrupt.  (could actually put it high?  need to
// put all the constants in one header file.
// use a define so can stick in .S file.

// if we don't put all this stuff in the libpi then we can't switch between network
// and not network b/c you'd have to hack all the makefiles.
//
// on the other hand, idk if it's a bright idea to put into libpi.  it's going to
// pull in all sorts of stuff if we're not careful.

#include "rpi-constants.h"
#include "sw-uart.h"

typedef enum { PUTC_HW_UART = 1, PUTC_SW_UART, PUTC_ESP_UART } uart_dev_t;
// typedef enum { ESP_NONE = 0, ESP_HW_UART , ESP_SW_UART } esp_io_device_t;

// various facts about the environmental setup when the process runs.
typedef struct {
    unsigned magic;

    // which device for console prints.
    uart_dev_t console_dev;

    // which pins if sw uart
    sw_uart_t sw_uart;

    // add anything you want to pass.

    // could add argv etc.
} control_blk_t;

// #define CTRL_BLK_ADDR (control_blk_t*)0x6000 

// we put it right above the interrupt stack (if any)
#define CTRL_BLK_ADDR (control_blk_t*)HIGHEST_USED_ADDR

#define CTRL_BLK_MAGIC 0x12345678

control_blk_t *cb_new(void);
control_blk_t *cb_get_block(void);
void cb_init(void);

#endif

