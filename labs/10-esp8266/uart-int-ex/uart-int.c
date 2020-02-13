/*
 * engler, cs140e: simple tests to check that you are handling rising and falling
 * edge interrupts correctly.
 *
 * NOTE: make sure you connect your GPIO 20 to your GPIO 21 (i.e., have it "loopback")
 */
#include "rpi.h"
#include "timer-interrupt.h"
#include "libc/circular.h"
#include "sw-uart.h"
#include "cycle-count.h"
#include "uart-int.h"

// client has to define this.
void interrupt_vector(unsigned pc) {
    uart_interrupt_handler();
}

void notmain() {
    int_init();
    uart_init_with_interrupts();
    system_enable_interrupts();

//    uart_init();
    printk("hello\n");
    delay_ms(100);
    
    while(1)  {
        system_disable_interrupts();
        while(uart_has_data_int()) 
            printk("%c", uart_getc_int());
        system_enable_interrupts();
        delay_ms(1000);
    }
#if 0
    nevents = 0;

    // this will cause transitions every time, so you can compared times.
    for(int l = 0; l < 2; l++) {
        unsigned b = 0b01010101;
        sw_uart_putc(&u, b);
        delay_ms(100);
        printk("nevent=%d\n", nevents);
        struct event * e = &events[0];
        for(int i = 0; i < nevents; i++,e++) 
            printk("\tv=%d, cyc=%d\n", e->v, e->ncycles);
        nevents = 0;
    }
#endif
    printk("SUCCESS!\n");
    clean_reboot();
}
