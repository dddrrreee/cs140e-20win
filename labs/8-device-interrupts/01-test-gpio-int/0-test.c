// trivial test of putk: this is the easiest one, since the uart hardware
// is pretty tolerant of errors.
#include "rpi.h"

volatile unsigned* DISABLE_IRQS_1 = (void*)0x2000B21C;
volatile unsigned* DISABLE_IRQS_2 = (void*)0x2000B220;

volatile unsigned* ENABLE_IRQS_1 = (void*)0x2000B210;
volatile unsigned* ENABLE_IRQS_2 = (void*)0x2000B214;

volatile unsigned int_flag = 0;

void interrupt_vector (unsigned pc) {
	int_flag = 1;
}

// need to setup interrupts, then call interrupt handler.
void notmain(void) {
    int pin = 20;

	// Enable GPIO
	gpio_init();
	
	// Disable interrupts
	put32(DISABLE_IRQS_1, 0xFFFFFFFF);
	put32(DISABLE_IRQS_2, 0xFFFFFFFF);

	gpio_set_input(pin);

	// Enable GPIO interrupts
	// Bit 52 handles GPIO, but split in two
	put32(ENABLE_IRQS_2, 1 << (52 - 32));

    gpio_int_rising_edge(pin);
    gpio_int_falling_edge(pin);

    gpio_event_detected(pin);
    gpio_event_clear(pin);

	while(int_flag == 0) {
		output("Waiting\n");
		delay_us(1000);
	}

    clean_reboot();
}
