/*************************************************************************
 * You need to implement these.
 */
#ifndef __GPIO_H__
#define __GPIO_H__

// GPIO pin mappings for UART
#define GPIO_TX 14
#define GPIO_RX 15

// different functions we can set GPIO pins to.
typedef enum {
    GPIO_FUNC_INPUT   = 0,
    GPIO_FUNC_OUTPUT  = 1,
    GPIO_FUNC_ALT0    = 4,
    GPIO_FUNC_ALT1    = 5,
    GPIO_FUNC_ALT2    = 6,
    GPIO_FUNC_ALT3    = 7,
    GPIO_FUNC_ALT4    = 3,
    GPIO_FUNC_ALT5    = 2,
} gpio_func_t;

// set GPIO function for <pin> (input, output, alt...).  settings for other
// pins should be unchanged.
void gpio_set_function(unsigned pin, gpio_func_t function);

// set <pin> to input or output.
void gpio_set_input(unsigned pin);
void gpio_set_output(unsigned pin);

// write <pin> = <val>.  1 = high, 0 = low.
// <pin> must be in output mode, other pins will be unchanged.
void gpio_write(unsigned pin, unsigned val);

// read <pin>: 1 = high, 0 = low.
int gpio_read(unsigned pin);

// turn <pin> on.
void gpio_set_on(unsigned pin);

// turn <pin> off.
void gpio_set_off(unsigned pin);

#endif
