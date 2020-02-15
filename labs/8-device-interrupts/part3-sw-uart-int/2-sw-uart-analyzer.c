/*
 * engler, cs140e: simple interrupt-based logic analyzer to check software uart 
 * implementation.
 *  1. do a sw_uart_putc
 *  2. as it does transitions, you will catch them in the interrupt handler.
 *  3. record the event and the cycle it occured.
 *  4. print at the end.
 *
 *  5. do the same for getc.
 *
 * NOTE: make sure you connect your GPIO 20 to your GPIO 21 (i.e., have it "loopback")
 */
#include "rpi.h"
#include "timer-interrupt.h"
#include "libc/circular.h"

volatile unsigned time_buffer [1024];
volatile time_index = 0;

enum { out_pin = 21, in_pin = 20 };


// client has to define this.
void interrupt_vector(unsigned pc) {
    dev_barrier();

	if(is_gpio_int(GPIO_INT0) || is_gpio_int(GPIO_INT1)) {
		time_buffer[time_index] = cycle_cnt_read();
		time_index++;
	}
	gpio_event_clear(in_pin);
}

void notmain() {
    uart_init();
	
    int_init();

    gpio_set_output(out_pin);
    gpio_set_input(in_pin);
    gpio_int_rising_edge(in_pin);
    gpio_int_falling_edge(in_pin);

    system_enable_interrupts();

    // setup sw uart
	sw_uart_t u = sw_uart_init(20, 21, 115200);

    //printk("do a sw_uart_putc and record the evants in a circular buffer\n");
	
	time_buffer[time_index] = cycle_cnt_read();
	time_index++;

	sw_uart_putc(u, 'f');

    //printk("do a sw_uart_get and record the evants in a circular buffer\n");
    time_buffer[time_index] = cycle_cnt_read();
	time_index++; 
	char in_char = sw_uart_getc(u);
	
	//printk("    (must modify so that you do a gpio_write to signal output\n");
    
	time_buffer[time_index] = cycle_cnt_read();
	time_index++;
	gpio_write(out_pin, 1);

	printk("putc time: %d", time_buffer[0] - time_buffer[1];);
	printk("putc time: %d", time_buffer[2] - time_buffer[3]

    printk("SUCCESS!\n");
    clean_reboot();
}
