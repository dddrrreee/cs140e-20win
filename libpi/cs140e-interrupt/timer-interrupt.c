/*
	simple timer interrupt demonstration.

      Good timer / interrupt discussion at:
 	http://www.valvers.com/open-software/raspberry-pi/step04-bare-metal-programming-in-c-pt4/

	Most of this code is from here.
*/
#include "rpi.h"
#include "timer-interrupt.h"

// macros to help deal with structures and put/get
#include "libc/helper-macros.h"

static void timer_check_offsets(void);

void timer_interrupt_init(unsigned ncycles) {
    timer_check_offsets();

    // from valvers:
    //	 Enable the timer interrupt IRQ
    PUT32(Enable_Basic_IRQs, RPI_BASIC_ARM_TIMER_IRQ);

    /* Setup the system timer interrupt */
    /* Timer frequency = Clk/256 * Load --- so smaller = more frequent. */
    PUT32(arm_timer_Load, ncycles);

    // Setup the ARM Timer: experiment by changing the prescale [defined on p197]
#if 1
    PUT32(arm_timer_Control,
            RPI_ARMTIMER_CTRL_32BIT |
            RPI_ARMTIMER_CTRL_ENABLE |
            RPI_ARMTIMER_CTRL_INT_ENABLE |
            RPI_ARMTIMER_CTRL_PRESCALE_16);
#else
    // example of how to use a structure instead.
    // structures often be better if you are doing a 
    // lot of read-modify-write.
    rpi_arm_timer_ctrl_t c = { 
            .use_32bit_counter = 1,
            .timer_enabled = 1,
            .int_enabled = 1, 
            .prescaler = 0b10,  // 256
    };
    PUT32_T(arm_timer_Control, c);

    // check that what was written is correct.
    // <Control> is the wrong type (uint32_t) so use <assign32_T>
    // to do the assignment; if it was correct could just do a
    //      v = get32_T(&RPI_GetArmTimer->Control);
    rpi_arm_timer_ctrl_t v;
    assign32_T(v, arm_timer_Control);
    assert(memcmp(&v,&c,sizeof c) == 0);
#endif

    // Now GPIO interrupts are enabled, but interrupts are still
    // globally disabled. Caller has to enable them when they are 
    // ready.
}

// Q: how does the libpi/libc/assert.h:AssertNow macro work?
// Q: what happens / why when you change some of these constants?
static void timer_check_offsets(void) {
    // I'm not sure how to check bitfield width statically, so the
    // unfortunately are runtime checks.
    //              <struct type>,        <field>,    <offset>, <nbits>
    check_bitfield(rpi_arm_timer_ctrl_t, use_32bit_counter,  1, 1);
    check_bitfield(rpi_arm_timer_ctrl_t, prescaler,      2, 2);
    check_bitfield(rpi_arm_timer_ctrl_t, int_enabled,        5, 1);
    check_bitfield(rpi_arm_timer_ctrl_t, timer_enabled,      7, 1);
    check_bitfield(rpi_arm_timer_ctrl_t, counter_enabled,    9, 1);
    check_bitfield(rpi_arm_timer_ctrl_t, counter_prescaler,          16, 7);
    AssertNow(sizeof(rpi_arm_timer_ctrl_t) == 4);
    printk("offsets checked out!\n");
}
