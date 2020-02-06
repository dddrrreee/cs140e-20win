// simple test for your gpio-int code: get this matched up first.
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "rpi.h"
#include "pi-test.h"

#define stringify(x) #x
#define test(fn) do { \
    printf("testing <%s>\n", stringify(fn)); \
    fn; \
} while(0)

int main(int argc, char *argv[]) {
    fake_pi_init();

    int pin = 20;
    test(gpio_int_rising_edge(pin));
    test(gpio_int_falling_edge(pin));
    test(gpio_event_detected(pin));
    test(gpio_event_clear(pin));
    test(is_gpio_int(GPIO_INT0));
    return 0;
}
