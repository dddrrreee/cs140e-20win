// when you run should blink 10 times. will have to restart the pi by pulling the
// usb connection out.
#include "gpio.h"

void notmain(void) {
    const int led = 20;
    const int touch = 16;

    gpio_set_output(led);
    gpio_set_input(touch);
    while(1) { 
        if(gpio_read(touch))
            gpio_set_on(led);
        else
            gpio_set_off(led);
    }
}
