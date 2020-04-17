#include "rpi.h"
#include "sw-uart.h"
#include "control-block.h"

#if 0
static sw_uart_t console_uart;
static int internal_sw_putchar(int c) {
    sw_uart_putc(&console_uart, c);
    return c;
}
#endif

void _cstart() {
    extern int __bss_start__, __bss_end__;
	void notmain();

    int* bss = &__bss_start__;
    int* bss_end = &__bss_end__;
 
    while( bss < bss_end )
        *bss++ = 0;

#if 0
    // this links in so much stuff.  need to fix.
    control_blk_t *cb = cb_get_block();
    if(cb) {
        if(cb->console_dev == PUTC_SW_UART) {
            rpi_putchar = internal_sw_putchar;
            console_uart = cb->sw_uart;
            sw_uart_printk(&console_uart, "about to print on regular printf\n");
            printk("success!\n");
        }
    }
#endif
    notmain(); 
	rpi_reboot();
}

