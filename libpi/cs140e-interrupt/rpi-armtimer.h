#ifndef __RPI_ARMTIMER_H__
#define __RPI_ARMTIMER_H__
/*
 * definition of the arm timer: ch 14, pages 196--199 of the broadcom doc.
 * NOTE: the "23_timer" in the broadcom doc is incorrect, it is a 32-bit 
 * timer.  see:
 *      https://elinux.org/BCM2835_datasheet_errata
 *
 * Note: the volatile, etc are unnecessary since we use PUT32/GET32.
 */

/** @brief 0 : 16-bit counters - 1 : 32-bit counter */
#define RPI_ARMTIMER_CTRL_32BIT         ( 1 << 1 )

#define RPI_ARMTIMER_CTRL_PRESCALE_1    ( 0 << 2 )
#define RPI_ARMTIMER_CTRL_PRESCALE_16   ( 1 << 2 )
#define RPI_ARMTIMER_CTRL_PRESCALE_256  ( 2 << 2 )

/** @brief 0 : Timer interrupt disabled - 1 : Timer interrupt enabled */
#define RPI_ARMTIMER_CTRL_INT_ENABLE    ( 1 << 5 )
#define RPI_ARMTIMER_CTRL_INT_DISABLE   ( 0 << 5 )

/** @brief 0 : Timer disabled - 1 : Timer enabled */
#define RPI_ARMTIMER_CTRL_ENABLE        ( 1 << 7 )
#define RPI_ARMTIMER_CTRL_DISABLE       ( 0 << 7 )

// timer control register: page 197 of broadcom.
// example of using bitfields in structures.  see <timer_check_offsets>
// for how to verify that we computed these offsets correctly.
typedef struct {
                                        // :<bit position> 
    uint32_t  :1,                       // :0 not used.  r/w
              use_32bit_counter:1,     // :1 1=use 32-bit counter, 0=16-bit
              prescaler:2,          // 2:3 pre-scale bits
                                        //   00 = clock / 1
                                        //   01 = clock / 16
                                        //   11 = clock/256
              :1,                       // :4 unused
              int_enabled:1,            // :5 1 = timer interrupt enabled
              :1,                       // :6 unused
              timer_enabled:1,          // :7 1 = timer enabled
              :1,                       // :8 ignore
              counter_enabled:1,        // :9 1 = free running counter enabled.
              :6,                       // 10:15 unused
              counter_prescaler:7;      // 16:23 free running counter prescaler.
                                        // reset value = 0x3e.
} rpi_arm_timer_ctrl_t;

/** @brief Section 14.2 of the BCM2835 Peripherals documentation details
    the register layout for the ARM side timer.  page 196 */
enum {
    arm_timer_Base = 0x2000B400,


    /** The timer load register sets the time for the timer to count down.
        This value is loaded into the timer value register after the load
        register has been written or if the timer-value register has counted
        down to 0. */
    arm_timer_Load = arm_timer_Base + 0,

    /** [Read-only] This register holds the current timer value and is counted down when
        the counter is running. It is counted down each timer clock until the
        value 0 is reached. Then the value register is re-loaded from the
        timer load register and the interrupt pending bit is set. The timer
        count down speed is set by the timer pre-divide register. */
    arm_timer_Value = arm_timer_Base + 4,

    /** The standard SP804 timer control register consist of 8 bits but in the
        BCM implementation there are more control bits for the extra features.
        Control bits 0-7 are identical to the SP804 bits, albeit some
        functionality of the SP804 is not implemented. All new control bits
        start from bit 8 upwards. */
    arm_timer_Control = arm_timer_Base + 8,

    /** [Write-only] The timer IRQ clear register is write only. When writing this 
        register the interrupt-pending bit is cleared. When reading this register it
        returns 0x544D5241 which is the ASCII reversed value for "ARMT". */
    arm_timer_IRQClear = arm_timer_Base + 12,

    /** [Read-only] The raw IRQ register is a read-only register. It shows the status of
        the interrupt pending bit. 0 : The interrupt pending bits is clear.
        1 : The interrupt pending bit is set.

        The interrupt pending bits is set each time the value register is
        counted down to zero. The interrupt pending bit can not by itself
        generates interrupts. Interrupts can only be generated if the
        interrupt enable bit is set. */
    arm_timer_RAWIRQ = arm_timer_Base + 16,

    /** [Read-only] The masked IRQ register is a read-only register. It shows the status
        of the interrupt signal. It is simply a logical AND of the interrupt
        pending bit and the interrupt enable bit. 0 : Interrupt line not
        asserted. 1 :Interrupt line is asserted, (the interrupt pending and
        the interrupt enable bit are set.)  */
    arm_timer_MaskedIRQ = arm_timer_Base + 20,

    /** This register is a copy of the timer load register. The difference is
        that a write to this register does not trigger an immediate reload of
        the timer value register. Instead the timer load register value is
        only accessed if the value register has finished counting down to
        zero. */
    arm_timer_Reload = arm_timer_Base + 24,

    /** The Pre-divider register is not present in the SP804. The pre-divider
        register is 10 bits wide and can be written or read from. This
        register has been added as the SP804 expects a 1MHz clock which we do
        not have. Instead the pre-divider takes the APB clock and divides it
        down according to:

        timer_clock = apb_clock/(pre_divider+1)

        The reset value of this register is 0x7D so gives a divide by 126. */
    arm_timer_PreDivider = arm_timer_Base + 28,

    /** The free running counter is not present in the SP804. The free running
        counter is a 32 bits wide read only register. The register is enabled
        by setting bit 9 of the Timer control register. The free running
        counter is incremented immediately after it is enabled. The timer can
        not be reset but when enabled, will always increment and roll-over.

        The free running counter is also running from the APB clock and has
        its own clock pre-divider controlled by bits 16-23 of the timer
        control register.

        This register will be halted too if bit 8 of the control register is
        set and the ARM is in Debug Halt mode. */
    arm_timer_FreeRunningCounter = arm_timer_Base + 32
} rpi_arm_timer_t;

void RPI_ArmTimerInit(void);
#endif
