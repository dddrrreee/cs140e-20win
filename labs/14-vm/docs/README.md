
`arm1176-vm.annot.pdf`: this is the chip specific chapter on vm.   Some notable
parts:

  - 6-9: disable, enable mmu
  - 6-10: set FCSE PID = 0 when disable.
  - 6-12: access permission table for apx and ap
        bits.
  - 6-12: execute never bit
  - 6-15: page table encodings of TEX, C, B (strongly
        ordered, device, etc)
  - 6-16: non-cacheable, write-back, etc. BB/AA bits.
  - 6-20: semantics of device memory / normal.
  - 6-21: can mark per page cacheable write through,
        cacheable write-back, non-cache.
  - 6-24: memory ordering table for the different flavors
  - 6-25: memory barrier discussion: dsb vs dmb
  - 6-29: table of mmu faults,
  - 6-30/6-31: flow chart of tlb fault checking
  - 6-34: encoding of fault register
  - 6-35: which registers updated on each fault type
  - 6-36: initial discussion of ttb0/ttb1
  - 6-39: what we do: page table translation, no sub pages (what w
        gives entry layout

  - 6-41: page coloring b/c of virtual indexed cache.
        i *think* we don't have to worry about
  - 6-42: detailed discussion of ttbr0, ttbcr
  - 6-46: translation figure for 1mb section.
  - 6-53: list of all mmu registers from cp15



`armv6.b2-memory.annot.pdf`: this gives memory ordering restrictions.
  - b2-13: memory model: what store must a load return?
  - b2-14: instruction fetches must only access normal memory.  does this mean
    we cannot mark ram as non-normal?
    further: prefetch can go down both
    branches: better make sure doesn't access device
    mem it should not!  
  - b2-16: table with rules for memory ordering
  - b2-17: rules for determining if access A1 occurs
    before A2. 
  - b2-18: memory barriers, dsb > dmb: dsb completes after all
    all memory, cache, tlb and branch prediction operations have completed
    no instruction after the dsb can execute until the dsb completes.
  - b2-19: prefetchflush: flushes the pipeline so all instructions after
    are fetched from cache/mem after prefetch completes.  ensure
    that changing asid, tlb etc are visible for sure after prefetch.
    all branches after the prefetch will have state after it.  other
    subtle points.
  - b2-20: possible operations needed to handle cache coherence issues
  - b2-21: cache management operations.
  * b2-22: tlb operations: this is crucial for us.     must 
    do a dsb + prefetchflush after you modify the TLB.
  * b2-23: if you modify the *page table* need to a dsb.  must 
    clean out of the cache if it's cacheable memory.  cookbook code
    to clean PTE modification.
  * b2-24: any invalidation of btb only visible after prefetch flush.
    list of operations where you need to do a flush:
        enable / disable mmu
        writing new code.
        writing pte entry
        changes to ttbr0, ttbr1, ttbcr
        changes to process id/context id.

    any change to cp14/cp15 need a prefetchflush.
   * b2-25: sync changes asid/ttbr.  cookbook example.  subtle.
     shows why *MUST* write the code in assembly, with no branches,
     no indirect jumps, no function calls.
        
