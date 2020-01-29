#include "rpi.h"
#include "sw-uart.h"

// helper: cleans up the code.
static inline void timed_write(int pin, int v, unsigned usec) {
    gpio_write(pin,v);
    delay_us(usec);
}

static inline char timed_read(int pin,  unsigned usec) {
    register unsigned input = gpio_read(pin) >> pin;
	//gpio_write(20, gpio_read(21) >> 21);
	delay_us(usec);
    return input;
}

// do this first: used timed_write to cleanup.
//  recall: time to write each bit (0 or 1) is in <uart->usec_per_bit>
inline void sw_uart_putc(sw_uart_t *uart, unsigned char c) {
    register unsigned time_delay = uart->usec_per_bit;
    register int pin = uart->tx;
    timed_write(pin, 0, time_delay);
    timed_write(pin, c & 1, time_delay);
    timed_write(pin, c & 2, time_delay);
    timed_write(pin, c & 4, time_delay);
    timed_write(pin, c & 8, time_delay);
    timed_write(pin, c & 16, time_delay);
    timed_write(pin, c & 32, time_delay);
    timed_write(pin, c & 64, time_delay);
    timed_write(pin, c & 128, time_delay);
    /*
    for(unsigned i = 0; i < 8; i++) {
        timed_write(pin, c & (1 << i), time_delay);
    } */
    timed_write(pin, 1, time_delay);
}

// do this second: you can type in pi-cat to send stuff.
//      EASY BUG: if you are reading input, but you do not get here in
//      time it will disappear.
inline int sw_uart_getc(sw_uart_t *uart, int timeout_usec) {
    //int timeout_count = 0;
    //register unsigned time_delay = uart->usec_per_bit;
    volatile int pin = uart->rx;
    register char output = 0;
    //volatile int first_read = 0; 
    register unsigned time_delay = uart->usec_per_bit;
    //register int pin = uart->tx;
	while (gpio_read(pin) >> (pin % 32) == 1) {
		;
	}
	
		/*	
        if(!first_read) {
			//sw_uart_putk(uart, "Waiting for falling edge\n");
			first_read = 1;
		} */
	//}
	//gpio_write(20, 0);
	delay_us(uart->usec_per_bit >> 1);
	delay_us(uart->usec_per_bit);
    /*
	timed_write(pin, 0, time_delay);
    timed_write(pin, 1, time_delay);
    timed_write(pin, 0, time_delay);
    timed_write(pin, 1, time_delay);
    timed_write(pin, 0, time_delay);
    timed_write(pin, 1, time_delay);
    timed_write(pin, 0, time_delay);
    timed_write(pin, 1, time_delay);
    timed_write(pin, 0, time_delay);
    timed_write(pin, 1, time_delay);
    */

	/*
	timed_write(20, 0, time_delay);
    timed_write(20, 1, time_delay);
    timed_write(20, 0, time_delay);
    timed_write(20, 1, time_delay);
    timed_write(20, 0, time_delay);
    timed_write(20, 1, time_delay);
    timed_write(20, 0, time_delay);
    timed_write(20, 1, time_delay);
    timed_write(20, 0, time_delay);
	timed_write(20, 1, time_delay);
	*/

	
	//timed_write(20, 0, time_delay);
	output |= timed_read(pin, time_delay);
	output |= timed_read(pin, time_delay) << 1;
	output |= timed_read(pin, time_delay) << 2;
	output |= timed_read(pin, time_delay) << 3;
	output |= timed_read(pin, time_delay) << 4;
	output |= timed_read(pin, time_delay) << 5;
	output |= timed_read(pin, time_delay) << 6;
	output |= timed_read(pin,  time_delay) << 7;
	//timed_write(20, 1, time_delay);
	

	/*
	char bitout;
	timed_write(20, 0, time_delay);
	for(int i = 7; i > 0; i--) {
		bitout = (timed_read(pin, time_delay) >> (pin % 32));
		gpio_write(20, 1);
		output |= bitout << i;
	}
	delay_us(time_delay);
	timed_write(20, 1, time_delay);
    */
	//if(timed_read(pin, time_delay) && 1) {

		/*
	output = (bit7 & 1) << 7 | \
             (bit6 & 1) << 6 | \
             (bit5 & 1) << 5 | \
             (bit4 & 1) << 4 | \
             (bit3 & 1) << 3 | \
             (bit2 & 1) << 2 | \
             (bit1 & 1) << 1 | \
             (bit0 & 1);
	}
	*/
    return output;
}

void sw_uart_putk(sw_uart_t *uart, const char *msg) {
    for(; *msg; msg++)
        sw_uart_putc(uart, *msg);
}




