/*
 * engler: trivial example to illustrate passing characters from interrupt to 
 * client code.
 *
 * this should look very familiar, since it's mostly a less chatty version of 
 * timer.c from the previous lab.
 */
#include "rpi.h"
#include "timer-interrupt.h"
#include "libc/circular.h"

static char hello[] = "hello world";
static cq_t putQ;
static volatile int ninterrupt;

// client has to define this.
void interrupt_vector(unsigned pc) {
    dev_barrier();
    unsigned pending = GET32(IRQ_basic_pending);

    // if this isn't true, could be a GPU interrupt (as discussed in Broadcom):
    // just return.  [confusing, since we didn't enable!]
    if((pending & RPI_BASIC_ARM_TIMER_IRQ) == 0)
        return;

    // clear timer.
    PUT32(arm_timer_IRQClear, 1);

    static int pos = 0;
    uint8_t c = hello[pos];
    if(c) { 
        cq_push(&putQ, c);
        pos++;
    }
    ninterrupt++;
    dev_barrier();    
}

void notmain() {
    uart_init();
	
    int_init();

    // if you mess with this value and prescaler, will change 
    // the number of interrupts.
    timer_interrupt_init(0x1);

    // Q: if you do this after enabling?
    cq_init(&putQ, 1);

    system_enable_interrupts();

    int n = strlen(hello);
    int i;

    printk("enabled: about to print string from int handler<");
    for(i = 0; i < n; i++) {
        uint8_t c = cq_pop(&putQ);
        if(!c)
            break;
        printk("%c", c);
    }
    printk(">\nDONE: read %d characters (strlen=%d)\n", i,n);
    printk("total interrupts = %d\n", ninterrupt);
    assert(n == i);
    printk("SUCCESS!\n");
    clean_reboot();
}
