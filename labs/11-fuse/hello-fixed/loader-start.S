// must use this start when loading code, otherwise we'll change the 
// stack.
.section ".text.boot"

.globl _start
_start:
    	b _loader_cstart
