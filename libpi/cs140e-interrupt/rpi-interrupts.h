#ifndef __RPI_INTERRUPT_H__
#define __RPI_INTERRUPT_H__

#include "rpi.h"

// from the valvers description.

/** @brief Bits in the Enable_Basic_IRQs register to enable various interrupts.
   See the BCM2835 ARM Peripherals manual, section 7.5 */
#define RPI_BASIC_ARM_TIMER_IRQ         (1 << 0)
#define RPI_BASIC_ARM_MAILBOX_IRQ       (1 << 1)
#define RPI_BASIC_ARM_DOORBELL_0_IRQ    (1 << 2)
#define RPI_BASIC_ARM_DOORBELL_1_IRQ    (1 << 3)
#define RPI_BASIC_GPU_0_HALTED_IRQ      (1 << 4)
#define RPI_BASIC_GPU_1_HALTED_IRQ      (1 << 5)
#define RPI_BASIC_ACCESS_ERROR_1_IRQ    (1 << 6)
#define RPI_BASIC_ACCESS_ERROR_0_IRQ    (1 << 7)

// bcm2835, p112   [starts at 0x2000b200]
enum { 
    IRQ_Base            = 0x2000b200,
    IRQ_basic_pending   = IRQ_Base+0,       // 0x200
    IRQ_pending_1       = IRQ_Base+4,       // 0x204
    IRQ_pending_2       = IRQ_Base+8,       // 0x208
    FIQ_control         = IRQ_Base+0xc,     // 0x20c
    Enable_IRQs_1       = IRQ_Base+0x10,    // 0x210
    Enable_IRQs_2       = IRQ_Base+0x14,    // 0x214
    Enable_Basic_IRQs   = IRQ_Base+0x18,    // 0x218
    Disable_IRQs_1      = IRQ_Base+0x1c,    // 0x21c
    Disable_IRQs_2      = IRQ_Base+0x20,    // 0x220
    Disable_Basic_IRQs  = IRQ_Base+0x24,    // 0x224
};

// bit-wise AND this with IRQ_basic_pending to see if it's a timer interrupt.
#define RPI_BASIC_ARM_TIMER_IRQ         (1 << 0)

void system_enable_interrupts(void);
void system_disable_interrupts(void);
void int_init(void);

#endif 
