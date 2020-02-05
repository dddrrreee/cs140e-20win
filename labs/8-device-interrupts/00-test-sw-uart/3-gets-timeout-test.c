// test of gets_timeout.   enter a bunch of text and hit return (so pi-cat sends back)
// or timeout.
#include "rpi.h"
#include "sw-uart.h"

void notmain(void) {
    uart_init();
    enable_cache();

    // use pin 20 for tx, 21 for rx
    sw_uart_t u = sw_uart_init(20,21, 115200);

    for(int i = 0; i < 4; i++) {
        unsigned timeout = 3;
        printk("%d: enter multiple lines, will stop reading when idle for=%dsec):\n", 
                        i,timeout);

        char buf[1024];
        // will read until the line is idle for a timeout period.
        int res = sw_uart_gets_timeout(&u, buf, sizeof buf, timeout*1000*1000);

        if(!res)
            printk("timeout: res=%d\n", res);
        else {
            assert(strlen(buf) == res);
            // kill newline.
            buf[res] = 0;
            printk("SW-UART: got nbytes=%d, string <%s>\n", res, buf);
        }
        delay_ms(2000);
    }
    clean_reboot();
}
