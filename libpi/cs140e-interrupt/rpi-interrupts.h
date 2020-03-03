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

// get the status register.
static inline unsigned cpsr_get(void) {
    unsigned cpsr;
    asm volatile("mrs %0,cpsr" : "=r"(cpsr));
    return cpsr;
}

static inline int int_is_enabled(void) {
    // 7ths bit = 1 ===> disabled.
    return ((cpsr_get() >> 7) & 1) == 0;
}

void system_enable_interrupts(void);
void system_disable_interrupts(void);
void int_init(void);

#if 0
/*
  p A2-16
  offsets for the different exceptions

  ldr pc, _reset_asm
  ldr pc, _undefined_instruction_asm
  ldr pc, _software_interrupt_asm
  ldr pc, _prefetch_abort_asm
  ldr pc, _data_abort_asm
  ldr pc, _reset_asm
  ldr pc, _interrupt_asm
*/
enum {
    INT_RESET_ID            = 0,
    INT_UNDEFINED_ID        = 1,
    INT_SWI_ID              = 2,
    INT_PREFETCH_ABORT_ID   = 3,
    INT_DATA_ABORT_ID       = 4,
    // there is no 5: mark as illegal
    INT_ILLEGAL_ID          = 5,
    INT_INTERRUPT_ID        = 6,
    INT_MAX_ID
};
// this is the intial trampoline: it has to setup a stack and save regs.
typedef void (*int_handler_t)(void);

// used to override the default int/exception handler.  must be
// called before int_init
void int_set_handler(unsigned handler_id, int_handler_t handler);
#endif

// need to disable mmu if it's enabled.
#define INT_UNHANDLED(msg,r) \
    panic("ERROR: unhandled exception <%s> at PC=%x\n", msg,r)

// can override these.
void data_abort_vector(unsigned pc);
void prefetch_abort_vector(unsigned pc);
void reset_vector(unsigned pc);
void interrupt_vector(unsigned pc);
void software_interrupt_vector(unsigned pc);

#endif 
