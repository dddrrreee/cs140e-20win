/* trivial blink */
#include "rpi.h"

// when you run should blink 10 times. will have to restart the pi by pulling the
// usb connection out.
void notmain(void) {
    uart_init();
    printk("about to start\n");
    int led = 20;

    gpio_set_output(led);
    for(int i = 0; i < 10; i++) {
        gpio_set_on(led);
        delay_ms(100);
        gpio_set_off(led);
        delay_ms(100);
    }
    printk("blink is done\n");
    clean_reboot();
}
