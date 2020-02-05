#ifndef __RPI_CONSTANTS__
#define __RPI_CONSTANTS__
/*
 * we put all the various magic constants here.   otherwise they are buried in different
 * places and it's easy to get conflicts.
 */

// this is 128MB --- should change to closer to what the r/pi A+ actually has.
#define STACK_ADDR          0x8000000

// put right above it.
#define INT_STACK_ADDR      0x9000000  

// this is the highest address we use --- above it is free.  should just rewrite 
// the code so that we can malloc.
#define HIGHEST_USED_ADDR INT_STACK_ADDR

#define MK_FN(fn_name)     \
.globl fn_name;             \
fn_name:

#define CYC_PER_USEC 700
#define PI_MHz  (700*1000*1000UL)

#endif
