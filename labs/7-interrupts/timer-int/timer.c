/*
	simple timer interrupt demonstration.

      Good timer / interrupt discussion at:
 	http://www.valvers.com/open-software/raspberry-pi/step04-bare-metal-programming-in-c-pt4/

	Most of this code is from here.

*/
#include "rpi.h"
#include "timer-interrupt.h"

// Q: if you make not volatile?
static volatile unsigned cnt;
static volatile unsigned period;

// client has to define this.
void interrupt_vector(unsigned pc) {
    unsigned pending = get32(&RPI_GetIRQController()->IRQ_basic_pending);

    // if this isn't true, could be a GPU interrupt: just return.
    // [confusing, since we didn't enable!]
    if((pending & RPI_BASIC_ARM_TIMER_IRQ) == 0)
        return;

    /* 
     * Clear the ARM Timer interrupt - it's the only interrupt we have
     * enabled, so we want don't have to work out which interrupt source
     * caused us to interrupt 
     *
     * Q: if we delete?
     */
    put32(&RPI_GetArmTimer()->IRQClear, 1);
    cnt++;

    static unsigned last_clk = 0;
    unsigned clk = timer_get_usec();
    period = last_clk ? clk - last_clk : 0;
    last_clk = clk;

    // Q: if we put a print statement?
}

void notmain() {
    uart_init();
	
    printk("about to install handlers\n");
    int_init();

    printk("setting up timer interrupts\n");
    // Q: if you change 0x100?
    timer_interrupt_init(0x100);

    printk("gonna enable ints globally!\n");
    
    // Q: if you don't do?
    system_enable_interrupts();
    printk("enabled!\n");

    // enable_cache(); 	// Q: what happens if you enable cache?
    unsigned iter = 0;
    while(cnt<200) {
        printk("iter=%d: cnt = %d, period = %dusec, %x\n", 
                                    iter,cnt, period,period);
        iter++;
    }
    clean_reboot();
}
