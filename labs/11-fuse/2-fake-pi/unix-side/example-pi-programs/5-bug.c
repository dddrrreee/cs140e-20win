/* bug3 */
#include "rpi.h"

void notmain(void) {
    uart_init();
    int led = 20;

    gpio_set_input(led);
    for(int i = 0; i < 10; i++) {
        gpio_set_on(led);
        delay_ms(100);
        gpio_set_off(led);
        delay_ms(100);
    }
}
