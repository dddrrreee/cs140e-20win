/*
 * simple timer interrupt demonstration.
 * Good timer / interrupt discussion at:
     http://www.valvers.com/open-software/raspberry-pi/step04-bare-metal-programming-in-c-pt4
 *
 * Search for "Q:" to find the various questions.   Change the code to answer.
 */
#include "rpi.h"
#include "timer-interrupt.h"

// Q: if you make not volatile?
static volatile unsigned cnt, period, period_sum;

// client has to define this.
void interrupt_vector(unsigned pc) {
    dev_barrier();
    unsigned pending = GET32(IRQ_basic_pending);

    // if this isn't true, could be a GPU interrupt (as discussed in Broadcom):
    // just return.  [confusing, since we didn't enable!]
    if((pending & RPI_BASIC_ARM_TIMER_IRQ) == 0)
        return;

    // Checkoff: add a check to make sure we have a timer interrupt
    // use p 113,114 of broadcom.

    /* 
     * Clear the ARM Timer interrupt - it's the only interrupt we have
     * enabled, so we don't have to work out which interrupt source
     * caused us to interrupt 
     *
     * Q: what happens, exactly, if we delete?
     */
    PUT32(arm_timer_IRQClear, 1);


    /*
     * We do not know what the client code was doing: if it was touching a 
     * different device, then the broadcom doc states we need to have a
     * memory barrier.   NOTE: we have largely been living in sin and completely
     * ignoring this requirement for UART.   (And also the GPIO overall.)  
     * This is probably not a good idea and we should be more careful.
     */
    dev_barrier();    
    cnt++;

    // compute time since the last interrupt.
    static unsigned last_clk = 0;
    unsigned clk = timer_get_usec();
    period = last_clk ? clk - last_clk : 0;
    period_sum += period;
    last_clk = clk;

    // Q: what happens (&why) if you uncomment the print statement?
    // printk("In interrupt handler at time: %d\n", clk);
}

#include "cs140e-src/cycle-count.h"
void notmain() {
    uart_init();
	
    printk("about to install handlers\n");
    int_init();

    printk("setting up timer interrupts\n");
    // Q: if you change 0x100?
    timer_interrupt_init(0x100);

    printk("gonna enable ints globally!\n");
    // Q: what happens (&why) if you don't do?
    system_enable_interrupts();
    printk("enabled!\n");

    unsigned start = timer_get_usec();

    // Q: what happens if you enable cache?  Why are some parts
    // the same, some change?
    //enable_cache(); 	
    unsigned iter = 0, sum = 0;
#   define N 20
    while(cnt < N) {
        // Q: if you comment this out?  why do #'s change?
#if 0
        printk("iter=%d: cnt = %d, time between interrupts = %d usec (%x)\n", 
                                    iter,cnt, period,period);
#endif
        iter++;
    }

    // overly complicated calculation of sec/ms/usec breakdown
    // make it easier to tell the overhead of different changes.
    // not really specific to interrupt handling.
    unsigned tot = timer_get_usec() - start,
             tot_sec    = tot / (1000*1000),
             tot_ms     = (tot / 1000) % 1000,
             tot_usec   = (tot % 1000);
    printk("-----------------------------------------\n");
    printk("summary:\n");
    printk("\t%10d: total iterations\n", iter);
    printk("\t%10d: tot interrupts\n", N);
    printk("\t%10d: iterations / interrupt\n", iter/N);
    printk("\t%10d: average period\n\n", period_sum/(N-1));
    printk("total execution time: %dsec.%dms.%dusec\n", 
                    tot_sec, tot_ms, tot_usec);

    printk("\nmeasuring cost of different operations (pi A+ = 700 cyc / usec)\n");
    cycle_cnt_init();
    TIME_CYC_PRINT("\tread/write barrier", dev_barrier());
    TIME_CYC_PRINT("\tread barrier", dmb());
    TIME_CYC_PRINT("\tsafe timer", timer_get_usec());
    TIME_CYC_PRINT("\tunsafe timer", timer_get_usec_raw());
    // macro expansion messes up the printk
    printk("\t<cycle_cnt_read()>: %d\n", TIME_CYC(cycle_cnt_read()));
    printk("-----------------------------------------\n");

    clean_reboot();
}
