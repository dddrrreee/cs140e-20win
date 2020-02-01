/*
	simple timer interrupt demonstration.

      Good timer / interrupt discussion at:
 	http://www.valvers.com/open-software/raspberry-pi/step04-bare-metal-programming-in-c-pt4/

	Most of this code is from here.
*/
#include "rpi.h"
#include "timer-interrupt.h"

volatile rpi_irq_controller_t* RPI_GetIRQController(void) {
    mb();
    return (void*)0x2000B200;
}

/** @brief See the documentation for the ARM side timer (Section 14 of the
    BCM2835 Peripherals PDF) */
volatile rpi_arm_timer_t* RPI_GetArmTimer(void) {
    mb();
    return (void*)0x2000B400;
}

void timer_interrupt_init(unsigned ncycles) {
    // from valvers:
    //	 Enable the timer interrupt IRQ
    put32(&RPI_GetIRQController()->Enable_Basic_IRQs, RPI_BASIC_ARM_TIMER_IRQ);

    /* Setup the system timer interrupt */
    /* Timer frequency = Clk/256 * Load --- so smaller = more frequent. */
    put32(&RPI_GetArmTimer()->Load, ncycles);

    // Setup the ARM Timer: experiment by changing the prescale [defined on p197]
    put32(&RPI_GetArmTimer()->Control,
            RPI_ARMTIMER_CTRL_23BIT |
            RPI_ARMTIMER_CTRL_ENABLE |
            RPI_ARMTIMER_CTRL_INT_ENABLE |
            RPI_ARMTIMER_CTRL_PRESCALE_256);

    // Now GPIO interrupts are enabled, but interrupts are still
    // globally disabled. Caller has to enable them when they are 
    // ready.
}
