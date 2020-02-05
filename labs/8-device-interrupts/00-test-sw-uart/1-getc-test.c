// test of getc: trickier since:
//      1. if we make timing mistakes can start missing input.
//      2. also we have to be listening when the input arrives.
#include "rpi.h"
#include "sw-uart.h"

void notmain(void) {
    uart_init();
    enable_cache();

    // use pin 20 for tx, 21 for rx
    sw_uart_t u = sw_uart_init(20,21, 115200);

    // just loop asking for one character.  enough delay that should work fine.
    for(int i = 0; i < 4; i++) {
        printk("%d: asking to print on sw-uart\n", i);

        unsigned timeout = 5;
        sw_uart_printk(&u, "%d: enter a character (timeout=%dsec): ", i,timeout);
        int res = sw_uart_getc_timeout(&u, timeout*1000*1000);
        if(res < 0)
            printk("timeout: res=%d!\n", res);
        else
            printk("\nSW-UART: got char <%c> (intval=%d, bits=%b)\n",
                            res, res, res);
        delay_ms(2000);
    }
    clean_reboot();
}
