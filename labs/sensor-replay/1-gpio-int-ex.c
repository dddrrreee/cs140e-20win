/*
 * engler, cs140e: simple tests to check that you are handling rising and falling
 * edge interrupts correctly.
 *
 * NOTE: make sure you connect your GPIO 20 to your GPIO 21 (i.e., have it "loopback")
 */

// Overall strategy: have Pi act as passthrough for frst iteration, but store incoming
// and outcoming traffic into circular queues with each entry timestamped. Then, 
//

#include "rpi.h"
#include "timer-interrupt.h"
#include "libc/circular.h"
#include "sw-uart.h"
#include "cycle-util.h"

static volatile int ninterrupt;

static cq_t recordDataQ;
static cq_t recordClkQ;
static cq_t replayDataQ;
static cq_t replayClkQ;

static volatile unsigned timeout = 0;
static volatile int in_write = 0;

static int record_scl = 6;
static int record_sda = 5;
static int replay_scl = 16;
static int replay_sda = 12;


// client has to define this.
void interrupt_vector(unsigned pc) {
    dev_barrier();
	//if(gpio_event_detected(record_scl)){
		if(0 == gpio_read(record_scl)) {
			gpio_write(replay_scl, 0);
		} else {
			gpio_write(replay_scl, 1);
		}
		gpio_event_clear(record_scl);
	//}

	//if(gpio_event_detected(record_sda)) {
		if(0 == gpio_read(record_sda)) {
			gpio_write(replay_sda, 0);
		} else {
			gpio_write(replay_sda, 1);
		}
		gpio_event_clear(record_sda);
	//}

	dev_barrier();
}

void init_recording(void) {
	gpio_set_input(record_scl);
	gpio_set_input(record_sda);
	gpio_set_output(replay_scl);
	gpio_set_output(replay_sda);

	gpio_int_rising_edge(record_scl);
	gpio_int_falling_edge(record_scl);
	gpio_int_rising_edge(record_sda);
	gpio_int_falling_edge(record_sda);

	gpio_write(replay_scl, 1);
	gpio_write(replay_sda, 1);

}

void notmain() {
    uart_init();
    
	enable_cache();
    
    int_init();
	
	init_recording();

    cq_init(&recordDataQ, 1);
    cq_init(&recordClkQ, 1);
    cq_init(&replayDataQ, 1);
    cq_init(&replayClkQ, 1);
    
	printk("Should see some output\n");

    system_enable_interrupts();

	// echo whatever we get in from the Arduino
	while(1){;}

	
	clean_reboot();
                         

}
