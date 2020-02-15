/*
 * engler, cs140e: simple echo program to test that you are doing uart interrupts
 * correctly.
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

void my_puts(const char *str) {
    for(; *str; str++)
        uart_putc_int(*str);
}

void notmain() {
    int_init();
    uart_init_with_interrupts();
    system_enable_interrupts();

    // easiest test: just use the hw-uart.
    my_puts("going to test using the hw-uart only:\n");
    while(1) {
        while(uart_has_data_int()) 
            uart_putc_int(uart_getc_int());
    }

#if 0
    // slightly more fancy: make sure printk works.
    my_puts("going to test using printk\n");
    // overwrite printk's putc method so that it calls our putk.
    rpi_set_putc(uart_putc_int);
    while(1) {
        while(uart_has_data_int()) 
            printk("%c", uart_getc_int());
    }
#endif

#if 0
    /* 
     * this tests using the sw-uart: note we likely have to 
     * disable interrupts to make sure the sw-uart can do its
     * timings.  however, if you tune your hw-uart interrupt 
     * handler you might be able to get rid of such tricks.
     *
     * if you aren't using the sw-bootloader need to setup the 
     * sw uart first.
     * 
     * you'll have to have pi-cat running in another window.
     */ 
    unsigned tx = 20, rx = 21;
    sw_uart_t s = sw_uart_init(tx,rx, 115200);

    printk("enter input with a carriage return\n");
    // do 10 lines.
    for(int i = 0; i < 10; ) {
        while(uart_has_data_int()) {
            int c = uart_getc_int();
            system_disable_interrupts();
            sw_uart_putc(&s, c);
            system_enable_interrupts();
            uart_putc_int(c);
            if(c == '\n')
                i++;
        }
    }
    printk("Done: handled %d lines\n");
#endif

    printk("SUCCESS!\n");
    clean_reboot();
}
