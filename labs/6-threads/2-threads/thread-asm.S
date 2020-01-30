/*
 * implement these.  ordered in difficulty.
 */
#include "rpi-asm.h"

@ return the current stack pointer.
MK_FN(rpi_get_sp)
	mov r0, r13
	bx lr

@empty stub routines.  use these, or make your own.
MK_FN(rpi_cswitch)
    
	stmfd r0!,{r4-r11}
	str r0, [r2]
	str r1, [r3]
	stmfd sp!, {r3,r14}
	bl rpi_init_trampoline
	ldmia sp!, {r3,r14}
	ldr r1, [r3]
	ldr r0, [r1]
	ldmia r0!,{r4-r11}
	bx lr

@ [Make sure you can answer: why do we need to do this?]
@
@ use this to setup each thread for the first time.
@ setup the stack so that when cswitch runs it will:
@	- load address of <rpi_init_trampoline> into LR
@	- <code> into r1, 
@	- <arg> into r0
@ 
MK_FN(rpi_init_trampoline)
	bx lr


#if 0
MK_FN(kmalloc)
    bx lr
MK_FN(kmalloc_init)
    bx lr
#endif
