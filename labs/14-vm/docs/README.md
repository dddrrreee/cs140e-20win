
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

