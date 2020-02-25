// must use this start when loading code, otherwise we'll change the 
// stack.
.section ".text.boot"

// Q: this start looks weird compared to cs140e-start.S: why?
.globl _start
_start:
    // Q: why is this a <b> rather than a <bl>?
    b _loader_cstart
