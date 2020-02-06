#include "rpi.h"

// need to setup interrupts, then call interrupt handler.
void notmain(void) {
    int pin = 20;
    gpio_int_rising_edge(pin);
    gpio_int_falling_edge(pin);

    gpio_event_detected(pin);
    gpio_event_clear(pin);

    clean_reboot();
}



