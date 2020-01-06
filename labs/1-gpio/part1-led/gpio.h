#ifndef __GPIO_H__
#define __GPIO_H__

// set <pin> to be an output pin.
void gpio_set_output(unsigned pin);

// set <pin> to input.
void gpio_set_input(unsigned pin);

// set GPIO <pin> on.
void gpio_set_on(unsigned pin);

// set GPIO <pin> off
void gpio_set_off(unsigned pin);

// set <pin> to <v> (v \in {0,1})
void gpio_write(unsigned pin, unsigned v);

// return the value of <pin>.
int gpio_read(unsigned pin);

#endif
