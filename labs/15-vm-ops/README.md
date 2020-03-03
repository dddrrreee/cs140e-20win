## Hooking translation up to hardware

Last lab we did the page table, the main noun of the VM universe.  This
lab we do the main gerunds needed to hook it up to the hardware: 

 - setting up domains.
 - setting up the page table register and ASID.
 - turning on the MMU.
 - making sure the state is coherent.  

You'll write assembly helper routines implement these (put them
in `vm-asm.s`).  Mechanically, you will go through, one-at-a-time
and replace every function prefixed with `our_` to be your own code.
The code is setup so that you can knock these off one at a time, making
sure that things work after each modification.

#### Flushing stale state.

The trickiest part of this lab is not figuring out the instructions 
to change the state we need, but is making sure you do --- exactly ---
the operations needed to flush all stale state throughout the machine. 
As mentioned in the previous lab, the hardware caches:
  - Page table entries (in the TLB).  If you change the page table,
  you need to flush the entries affected.
  - Memory (in both data and instruction caches).  If you change a virtual
  mapping or change addresses, you need to flush all affected entries.
  - The ARM optionally caches branch targets in a "branch target buffer"
  (`BTB`) so that it can predict execution paths better.  Unlike TLB
  entries, these entries are not tagged with an address space identifier
  (`ASID`).  Thus, you need to flush the BTB on almost all VM changes.
  - Further, ARM prefetches instructions: if you change a translation
  or change the address space you are in, the instructions in the 
  buffer are then almost certainly wrong, and you need to flush the
  prefetch buffer.
  - Finally, when you flush a cache or modify ARM co-processor state,
  there is often no guarantee that the operation has completed when the
  instruction finishes!  So you need to insert barriers.

Mistakes in the above are incredibly, incredibly nasty.  I believe if
you have one today, you will never track it down before the quarter ends
(I'd be surprised if there were more than 10 people in our building that
could such bugs):

  1. If you get it wrong, your code will likely "work" fine today.  We are
  running with caches disabled, no branch prediction, and strongly-ordered
  memory accesses so many of the gotcha's can't come up.  However, they
  will make a big entrance later when we are more aggressive about speed.
  And since at that point there will be more going on, it will be hard
  to figure out WTH is going wrong.

  2. Because flaws relate to memory --- what values are returned from
  a read, or what values are written --- they give "impossible" bugs
  that you won't even be looking for, so won't see.  (E.g., a write to a
  location disappears despite you staring right at the store that does it,
  a branch is followed the wrong way despite its condition being true).
  They are the ultimate, Godzilla-level memory corruption.

Thus, as in the `uart` lab, you're going to have to rely very strongly
on the documents from ARM and find the exact prose that states the exact
sequence of (oft non-intuitive) actions you have to do.  

These advices are consolidated towards the end of Section B2 of the
ARMv6 manual (`docs/armv6.b2-memory.annot.pdf`).  Useful pages:
  - B2-25: how to change the address space identifier (ASID). 
  - B6-22: all the different ways to flush caches, memory barriers (various
    snapshots below).  As you figured out in the previous lab, the r/pi A+
    we use has split instruction and data caches.
  - B2-23: how to flush after changing a PTE.
  - B2-24: must flush after a CP15.

#### Check-off

You're going to write a tiny amount of code (< 10 lines for each part),
but it has to be the right code.  You will:

  1. Set up domains.  You must put in barriers correctly (give the page
  number and what the ARM requires).  Show the code faults when you
  (1) run code for memory that has the "execute never" bit set, (2)
  write to memory that is read-only.

  2. Set the ASID and page-table pointer.  You must handle coherence
  correctly, using the ARM provided instruction sequence to switch
  `ASID`s (your comments should state page number citations and succinct
  intuition).

  3. Turn on/turn off the MMU.  As in (2) you must handle coherence /
  flushing correctly (with page number citations).

  4. Delete our files and starter code (remove references from the
  `Makefile`).  At this point, all code is written by you!

Extensions:
  1. Set up code so that it cleans the cache rather than just invalidates.
  2. Write code to make it easy to look up a PTE (`mmu_lookup_pte(void *addr)`)
  and change permissions, write-buffer, etc.
  3. Set-up two-level paging.
  4. Set-up 16MB paging.

----------------------------------------------------------------------
## Part 1: setting up domains.

Deliverables:
  1. You should replace `our_write_domain_access_ctrl` with yours.
  Make sure you obey any requirements for coherence stated in Chapter B2,
  specifically B2-24.  Make sure the code still works!

  2. Change the domain call so that permission checking works (why did
  it not work before?) and that code now crashes when it (1) executes a
  location we do not allow execution of, (2) writes to a location that
  has writes disabled.

Useful pages:
  - B4-10: what the bit values mean for the `domain` field.
  - B4-15: how addresses are translated and checked for faults.
  - B4-27: the location / size of the `domain` field in the segment 
  page table entry.
  - B4-42: setting the domain register.

Useful intuition:
  - When you flush the `BTB`, you need to do a `DSB` to wait for
  it to complete, and then a `Prefetch` flush.
  - before you do a flush, you need to do a `DSB` to ensure the 
  modifications that caused you to do it have completed.

#### Some intuition and background on domains.

ARM has an interesting take on protection.  Like most modern architectures
it allows you to mark pages as invalid, read-only, read-write, executable.
However, it gives you a way to quickly disable these restrictions in a
fine-grained way through the use of domains.

Mechanically it works as follows.
  - each page-table entry (PTE) has a 4-bit field stating which single 
  domain the entry belongs to.

  - the system control register (CP15) has a 32-bit domain register (`c3`,
  page B4-42) that contains 2-bits for each of the 16 domains stating
  what mode each the domain is in.  
    - no-access (`0b00`): no load or store can be done to any virtual
    address belonging to the domain;

    - a "client" (`0b01`): all accesses must be consistent with the
    permissions in their associated PTE;

    - a "manager" (`0b11`): no permission checks are done, can read or
    write any virtual address in the PTE region.

  - B4-15: On each memory reference, the hardware looks up the page
  table entry (in reality: the cached TLB entry) for the virtual address,
  gets the domain number, looks up the 2-bit state of the domain in the
  domain register checks if it is allowed.


As a result, you can quickly do a combination of both removing all access
to a set of regions, and granting all access to others by simply writing
a 32-bit value to a single coprocessor register.

To see how these pieces play together, consider an example where code
with more privileges (e.g., the OS) wants to run code that has less
privileges using the same address translations (e.g., a device driver
it doesn't trust).
   - The OS assigns the device driver a unique domain id (e.g., `2`).
   - The OS tags all PTE entries the driver is allowed to touch with `2`
   in the `domain` field.
   - When the OS is running it sets all domains to manager (`0b11`) mode
   so that it can read and write all memory.
   - When the OS wants to call the device driver, it switches the state of
   domain `2` to be a client (`0b01`) and all other domains as no-access
   (`0b00`).

Result:
  1. When the driver code runs, it cannot corrupt any other kernel memory.
  2. Switching domains is fast compared to switching page tables (the
  typical approach).  
  3. As a nice bonus: All the addresses are the same in both pieces of
  code, which makes many things easier.

In terms of our data structures: 
  - Assume we gave the driver access to the single virtual segment range
  `[0x100000, 0x20000)`, which is segment 1 (`0x100000 >> 20 = 1`).
  
  - We would set the `domain` field for the associated page table entry 
  containing the first level descriptor to `2`: i.e., `pt[1].domain = 2`.

  - Before starting the device driver we would write `0b01 << 2` into
  register 3 of CP15.   I.e., domain 2 is in client mode, all other
  domains (`0, 1, 3..15`) are in no-access mode.

----------------------------------------------------------------------
##### B4-32: Bits to set in Domain
<table><tr><td>
  <img src="images/part2-domain.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### B6-21: Flush prefetch buffer and tricks.

<table><tr><td>
  <img src="images/part3-flushprefetch.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### B6-22: DSB, DMB instruction

<table><tr><td>
  <img src="images/part3-dsb-dmb.png"/>
</td></tr></table>

----------------------------------------------------------------------
----------------------------------------------------------------------
## Part 2: Implement `set_procid_ttbr0`

Deliverable:
   - Set the page table pointer and address space identifier by replacing
   `our_set_procid_ttbr0` with yours.  Make sure you can switch between
   multiple address spaces.

Where and what:

  1. B4-41: The hardware has to be able to find the page table when
  there is a TLB miss.  You will write the address of the page table to
  the page table register `ttbr0`, set both `TTBR1` and `TTBRD` to `0`.
  Note the alignment restriction!

  2.  B4-52: The ARM allows each TLB entry to be tagged with an address
  space identifier so you don't have to flush when you switch address
  spaces.  Set the current address space identifier (pick a number between
  `1..63`).

  3. Coherence requirements: B2-21, B2-22, B2-23, B2-24 rules for changing
  page table register. And B2-25 the cookbook for changing an `ASID`.

----------------------------------------------------------------------
##### B4-41: Setting page table pointer.

<table><tr><td>
  <img src="images/part2-control-reg2-ttbr0.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### B2-25: Sync ASID

<table><tr><td>
  <img src="images/part3-sync-asid.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### B2-22: When do you need to flush 

<table><tr><td>
  <img src="images/part3-tlb-maintenance.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### B2-23: How to invalidate after a PTE change

<table><tr><td>
  <img src="images/part3-invalidate-pte.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### B2-24: When to flush BTB

<table><tr><td>
  <img src="images/part3-flush-btb.png"/>
</td></tr></table>

----------------------------------------------------------------------
----------------------------------------------------------------------
## Part 3: implement `mmu_enable` and `mmu_disable`

Now you can write the code to turn the MMU on/off:
  - `mmu_enable()`
  - `mmu_disable()`

The high-level sequence is given on page 6-9 of the `arm1176.pdf` document
(screen shot below).  You will also have to flush:
   - all caches (D/I cache, the I/D TLBs)
   - PrefetchBuffer.
   - BTB
   - and wait for everything correctly.

We provided macros for most of these; but you should check that they 
are correct.  

   * Note that the flush instruction cache operation has bugs in many
   in some ARM v6 chips, so we provided the recommended sequences (taken
   from Linux).

----------------------------------------------------------------------
##### 6-9: Protocol for turning on MMU.

<table><tr><td>
  <img src="images/part2-enable-mmu.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### B4-39 and B4-40: Bits to set to turn on MMU

<table><tr><td>
  <img src="images/part2-control-reg1.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### B6-21: Various invalidation instructions

<table><tr><td>
  <img src="images/part2-inv-tlb.png"/>
</td></tr></table>

----------------------------------------------------------------------
----------------------------------------------------------------------
## Part 4: Get rid of our code.

You should go through and delete all of our files, changing the `Makefile`
to remove references to them.  At this point, all code is written by you!


