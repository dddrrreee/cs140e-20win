#include "rpi-asm.h"

MK_FN(syscall_hello)
    @ we are already at system level: running this will trash
    @ the lr, so we need to save it.
    push {lr}
    swi 1
    pop {lr}
    bx lr

@ your code should reject this call with an error.
MK_FN(syscall_illegal)
    push {lr}
    swi 2
    pop {lr}
    bx lr
