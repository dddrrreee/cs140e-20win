// trivial test of putk: this is the easiest one, since the uart hardware
// is pretty tolerant of errors.
#include "rpi.h"
#include "sw-uart.h"

void notmain(void) {
    uart_init();
    enable_cache();

    // use pin 20 for tx, 21 for rx
    sw_uart_t u = sw_uart_init(20,21, 115200);

    for(int i = 0; i < 10; i++) {
        printk("%d: going to print hello world\n", i);
        sw_uart_printk(&u, "hello world: %d\n", i);
        delay_ms(1000);
    }

    clean_reboot();
}
