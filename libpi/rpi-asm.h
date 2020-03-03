#ifndef __RPI_ASM_H__
#define __RPI_ASM_H__
// only include this in .S files.

#include "rpi-constants.h"

#define MK_FN(fn_name)     \
.globl fn_name;             \
fn_name:


// used to make a cp15 function: clear the input "read-only" register
#define MK_CP15_FUNC(name, macro_name)           \
.globl name;                                \
name:                                       \
    CLR(r0);                                 \
    macro_name(r0);                          \
    bx lr


// used to clear register before CP15 operation.
#define CLR(reg) mov reg, #0 


#endif
