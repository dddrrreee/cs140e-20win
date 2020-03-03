/*
 * implement these.  ordered in difficulty.
 */
#include "rpi-asm.h"

@empty stub routines.  use these, or make your own.
MK_FN(rpi_cswitch_preemptive_nonbanked)
    STMIA r0, {r4-r14} @potentially don't need to save this many regs. What do about sp?
    LDMIA r1, {r4-r14} @see above. Also what to do about lr^ in this case?
    bx lr

@ PROBLEMS:
@ - [ ] Stack is not where you think it is, because due to call to rpi_cswitch_preemtive more things have been pushed to stack
@ 
@
MK_FN(rpi_cswitch_preemptive_banked)
    @save; from experiments, found that sp, #32 is where r0 from old context is saved; now 
    ldr r2, [sp, #16]
    str r2, [r0] @ store r0 old
    ldr r2, [sp, #20]
    str r2, [r0, #4] @ store r1 old
    ldr r2, [sp, #24]
    str r2, [r0, #8] @ store r2 old
    ldr r2, [sp, #28]
    str r2, [r0, #12] @ store r3 old
    ldr r2, [sp, #32]
    str r2, [r0, #16] @ store r4 old
    ldr r2, [sp, #36]
    str r2, [r0, #20] @ store r5 old
    ldr r2, [sp, #40]
    str r2, [r0, #24] @ store r6 old
    ldr r2, [sp, #44]
    str r2, [r0, #28] @ store r7 old
    ldr r2, [sp, #48]
    str r2, [r0, #32] @ store r8 old
    ldr r2, [sp, #52]
    str r2, [r0, #36] @ store r9 old
    ldr r2, [sp, #56]
    str r2, [r0, #40] @ store r10 old
    ldr r2, [sp, #60]
    str r2, [r0, #44] @ store r11 old
    ldr r2, [sp, #64]
    str r2, [r0, #48] @ store r12 old. These were all ugly. Is there better way?
    add r0, r0, #52
    LDMIA r0, {r13-r14}^ @store banked r13 and r14; perhaps there's a better way to do this...
    sub r0, r0, #52
    ldr r2, [sp, #68]
    str r2, [r0, #60] @store lr old, which is the old pc. Def cleaner way for this
    mrs r2, spsr
    str r2, [r0, #64] @save the old spsr to stack

    @load
    ldr r2, [r1] 
    str r2, [sp, #16] @ inject r0 new onto stack
    ldr r2, [r1, #4] 
    str r2, [sp, #20] @ inject r1 new onto stack
    ldr r2, [r1, #8] 
    str r2, [sp, #24] @ inject r2 new onto stack
    ldr r2, [r1, #12] 
    str r2, [sp, #28] @ inject r3 new onto stack @perhaps why this creates reset vector issue is b/c 28 is actually position of lr in some way?
    ldr r2, [r1, #16] 
    str r2, [sp, #32] @ inject r4 new onto stack
    ldr r2, [r1, #20] 
    str r2, [sp, #36] @ inject r5 new onto stack
    ldr r2, [r1, #24] 
    str r2, [sp, #40] @ inject r6 new onto stack
    ldr r2, [r1, #28]
    str r2, [sp, #44] @ inject r7 new onto stack
    ldr r2, [r1, #32] 
    str r2, [sp, #48] @ inject r8 new onto stack
    ldr r2, [r1, #36]
    str r2, [sp, #52] @ inject r9 new onto stack
    ldr r2, [r1, #40]
    str r2, [sp, #56] @ inject r10 new onto stack
    ldr r2, [r1, #44]
    str r2, [sp, #60] @ inject r11 new onto stack
    ldr r2, [r1, #48]
    str r2, [sp, #64] @ inject r12 new onto stack
    add r1, r1, #52
    STMIA r1, {r13-r14}^ @load banked r13 and r14; perhaps there's a better way to do this...
    sub r1, r1, #52
    ldr r2, [r1, #60] 
    str r2, [sp, #68] @inject lr new onto stack, which is the new pc. Def cleaner way for this
    ldr r2, [r1, #64]
    msr spsr, r2 @inject in new spsr... what to do on the first time???

    @ return back to interrupt_vector
    bx lr

MK_FN(rpi_init_trampoline_preemptive)
    mov r1,r4
    mov r0,r5
    blx r1 @ run code
    mov r0,#1
    bl rpi_exit_preemptive

