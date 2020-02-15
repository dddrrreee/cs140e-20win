// test of gets_until: enter lines in pi-cat.  if you hit newline will send to the pi
#include "rpi.h"
#include "sw-uart.h"

void notmain(void) {
    uart_init();
    enable_cache();

    // use pin 20 for tx, 21 for rx
    sw_uart_t u = sw_uart_init(20,21, 115200);

    for(int i = 0; i < 4; i++) {
        unsigned timeout = 5;
        printk("iter=%d, going to do a get of a whole line (timeout=%d)\n", i, timeout);

        sw_uart_printk(&u, "enter a line: ");

        char buf[1024];
        int res = sw_uart_gets_until(&u, buf, sizeof buf, '\n', timeout*1000*1000);

        if(res <= 0)
            printk("timeout: res=%d!!\n", res);
        else {
            assert(strlen(buf) == res);
            // kill newline.
            buf[res-1] = 0;
            printk("SW-UART: got string <%s>\n", buf);
        }
    }
    clean_reboot();
}
