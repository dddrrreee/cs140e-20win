/*
 * implement these.  ordered in difficulty.
 */
#include "rpi-asm.h"

@ return the current stack pointer.
MK_FN(rpi_get_sp)
	bx lr

@empty stub routines.  use these, or make your own.
MK_FN(rpi_cswitch)
    @ put your cswitch here.
	str r4, [r0]
	str r5, [r0, #4]
	str r6, [r0, #8]
	str r7, [r0, #12]
	str r8, [r0, #16]
	str r9, [r0, #20]
	str r10, [r0, #24]
	str r11, [r0, #28]
	str r12, [r0, #32]
	str r14, [r0, #36]
	str r13, [r0, #40]
	
	ldr r4, [r1]
	ldr r5, [r1, #4]
	ldr r6, [r1, #8]
	ldr r7, [r1, #12]
	ldr r8, [r1, #16]
	ldr r9, [r1, #20]
	ldr r10, [r1, #24]
	ldr r11, [r1, #28]
	ldr r12, [r1, #32]
	ldr r14, [r1, #36]
	ldr r13, [r1, #40]
	
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
	mov r0, r5 
	blx r4
	bl rpi_exit
