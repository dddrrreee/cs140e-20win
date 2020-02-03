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
typedef struct {
    uint32_t    IRQ_basic_pending,  // 0x200
                IRQ_pending_1,      // 0x204
                IRQ_pending_2,      // 0x208
                FIQ_control,        // 0x20c
                Enable_IRQs_1,      // 0x210
                Enable_IRQs_2,      // 0x214
                Enable_Basic_IRQs,  // 0x218
                Disable_IRQs_1,     // 0x21c
                Disable_IRQs_2,     // 0x220
                Disable_Basic_IRQs; // 0x224
} rpi_irq_controller_t;

volatile rpi_irq_controller_t* RPI_GetIRQController(void);

// and this with IRQ_basic_pending to see if it's a timer interrupt.
#define RPI_BASIC_ARM_TIMER_IRQ         (1 << 0)

void system_enable_interrupts(void);
void system_disable_interrupts(void);

// location of these registers.
#define INTERRUPT_ENABLE_1  0x2000b210
#define INTERRUPT_ENABLE_2  0x2000b214
#define INTERRUPT_DISABLE_1 0x2000b21c
#define INTERRUPT_DISABLE_2 0x2000b220

void int_init(void);

#endif 

