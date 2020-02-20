/* bug2: should flag! */
#include "rpi.h"
void notmain(void) {
    uart_init();
    printk("about to start\n");
    int led = 20;
    for(int i = 0; i < 10; i++) {
        gpio_set_on(led);
        delay_ms(100);
        gpio_set_off(led);
        delay_ms(100);
    }
}
