.globl flush_all_caches
flush_all_caches:
    mov r0, #0
    mcr p15, 0, r0, c7, c5, 0   // Invalidate instruction cache
    mcr p15, 0, r0, c7, c5, 6   // Invalidate BTB
    mcr p15, 0, r0, c7, c10, 4  // Drain write buffer
    mcr p15, 0, r0, c7, c5, 4   // Prefetch flush
        bx lr

