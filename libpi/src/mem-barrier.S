@ call to sync across different GPIO devices.
.globl dev_barrier
dev_barrier:
    b dsb

/* Data memory barrier */
.globl dmb
dmb:
    mov r0, #0
    mcr p15, 0, r0, c7, c10, 5
    bx lr

/*
 * used to be called "drain write buffer"
 * includes all cache operations.  is a superset (>) DMB
 */
.globl dsb
dsb:
        mcr p15, 0, r0, c7, c10, 4
        bx lr
