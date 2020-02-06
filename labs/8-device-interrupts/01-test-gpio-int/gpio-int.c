// engler, cs140 put your gpio implementations in here.
#include "rpi.h"

volatile unsigned* IRQ_PENDING_1 = (void*)0x2000B204;
volatile unsigned* IRQ_PENDING_2 = (void*)0x2000B208;

volatile unsigned* GPEDS0 = (void*)0x20200040;
volatile unsigned* GPEDS1 = (void*)0x20200044;

volatile unsigned* GPREN0 = (void*)0x2020004C;
volatile unsigned* GPREN1 = (void*)0x20200050;

volatile unsigned* GPFEN0 = (void*)0x20200058;
volatile unsigned* GPFEN1 = (void*)0x2020005C;

volatile unsigned* ENABLE_IRQS_2 = (void*)0x2000B214;


// gpio_int_rising_edge and gpio_int_falling_edge (and any other) should
// call this routine (you must implement) to setup the right GPIO event.
// as with setting up functions, you should bitwise-or in the value for the 
// pin you are setting with the existing pin values.  (otherwise you will
// lose their configuration).  you also need to enable the right IRQ.   make
// sure to use device barriers!!
int is_gpio_int(unsigned gpio_int) {
	// assert(gpio_int >= GPIO_INT0 && gpio_int <= GPIO_INT3);
	if(get32(IRQ_PENDING_2) & 1<<(gpio_int-GPIO_INT0 + 1)> 0) {
		return 1;
	}
	return 0;
}


// p97 set to detect rising edge (0->1) on <pin>.
// as the broadcom doc states, it  detects by sampling based on the clock.
// it looks for "011" (low, hi, hi) to suppress noise.  i.e., its triggered only
// *after* a 1 reading has been sampled twice, so there will be delay.
// if you want lower latency, you should us async rising edge (p99)
void gpio_int_rising_edge(unsigned pin) {
	
	
	if(pin >= 32) {
		return;
	}
	
	dev_barrier();
	if(pin <= 31) {
		unsigned bitmask = get32(GPREN0);
		bitmask |= 1 << pin;
		put32(GPREN0, bitmask);
	} else {
		unsigned bitmask = get32(GPREN1);
		bitmask |= 1 << (pin % 32);
		put32(GPREN1, bitmask);
	}
	dev_barrier();
	unsigned int_mask = get32(ENABLE_IRQS_2);
	int_mask |= 1 << (49 - 32);
	put32(ENABLE_IRQS_2, int_mask);
	dev_barrier();
}

// p98: detect falling edge (1->0).  sampled using the system clock.  
// similarly to rising edge detection, it suppresses noise by looking for
// "100" --- i.e., is triggered after two readings of "0" and so the 
// interrupt is delayed two clock cycles.   if you want  lower latency,
// you should use async falling edge. (p99)
void gpio_int_falling_edge(unsigned pin) {
	
	if(pin >= 32) {
		return;
	}
	
	dev_barrier();
	if(pin <= 31) {
		unsigned bitmask = get32(GPFEN0);
		bitmask |= 1 << pin;
		put32(GPFEN0, bitmask);
	} else {
		unsigned bitmask = get32(GPFEN1);
		bitmask |= 1 << (pin % 32);
		put32(GPFEN1, bitmask);
	}
	dev_barrier();
	unsigned int_mask = get32(ENABLE_IRQS_2);
	int_mask |= 1 << (52 -32);
	put32(ENABLE_IRQS_2, int_mask);
	dev_barrier();

}

// p96: a 1<<pin is set in EVENT_DETECT if <pin> triggered an interrupt.
// if you configure multiple events to lead to interrupts, you will have to 
// read the pin to determine which caused it.
int gpio_event_detected(unsigned pin) {
	if(pin >= 32) {
		return -1;
	}
	
	dev_barrier();
	if(pin <= 31) {
		unsigned reg_stat = get32(GPEDS0) & (1 << pin);
		if(reg_stat > 0) {dev_barrier(); return 1;}
	} else {
		unsigned reg_stat = get32(GPEDS1) & (1 << pin);
		dev_barrier();
		if(reg_stat > 0) {dev_barrier(); return 1;}
	}
	dev_barrier();
	return 0;
}

// p96: have to write a 1 to the pin to clear the event.
void gpio_event_clear(unsigned pin) {
    
	if(pin >= 32) {
		return;
	}
	
	dev_barrier();
	if(pin <= 31) {
		put32(GPEDS0, 1 << pin);
	} else {
		put32(GPEDS1, 1 << (pin % 32));
	}
	dev_barrier();
}
