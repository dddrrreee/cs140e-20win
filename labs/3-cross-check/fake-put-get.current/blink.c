// program that should run, unaltered, on both the pi and our fake pi environment.
#include "rpi.h"

void nop(void);

// countdown 'ticks' cycles; the asm probably isn't necessary.
void delay(unsigned ticks) {
    while(ticks-- > 0)
        nop();
}

void notmain(void) {
    uart_init();
    printk("about to start blink!\n");

    int led1 = 20;
    int led2 = 21;
    gpio_set_output(led1);
    gpio_set_output(led2);

    for(int i = 0; i < 10; i++) {
        gpio_set_on(led1);
        gpio_set_off(led2);
        delay(1000000);

        gpio_set_off(led1);
        gpio_set_off(led2);
        delay(1000000);
    }
    clean_reboot();
}
