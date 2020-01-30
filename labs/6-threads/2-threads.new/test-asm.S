/*
 * implement these.  ordered in difficulty.
 */
#include "rpi-asm.h"

@ return the current stack pointer.
MK_FN(callee_save)
	bx lr

@ store the second parameter into the memory 
@ pointed to by the first parameter
MK_FN(store_one_asm)
    bx lr

@ implement this routine
MK_FN(check_callee_save)
    @ first: destroy all registers by loading 4 into r4, 
    @ 5 into r5 etc for each saved register.



    @ second: save the registers: since we set them to known 
    @ values we can print them and check everything got
    @ written.

    @ call print_and_die


@
@ simple context switch:
@     cswitch(uint32_t *old, uint32_t *new)
@
MK_FN(cswitch)
    @ put your save code here

    @ kill all the regs to verify save/restore works.
    mov r4, #4
    mov r5, #5
    mov r6, #6
    mov r7, #7
    mov r8, #8
    mov r9, #9
    mov r10, #10
    mov r11, #11
    mov r12, #12
    mov sp, #13
    mov lr, #14
    
    @ put your restore code here.
    bx lr
