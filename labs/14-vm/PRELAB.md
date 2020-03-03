Virtual memory is the unknown left in our universe, so we'll bang
out quick a version, and then circle back around and make your system
more real.

You will need to do this reading.  The next two labs are the most "adult"
of this quarter.  While the fundamental thing we are doing is not that
complicated; though there are a fair number of details and a single
mistake leads bad bugs.  (Eg., an infinite exception handler loop where
you keep faulting over and over again, or a weird memory corruption
where you thought you had mapped a virtual address to one location,
but it was some other one, which gets trashed.)

#### The big picture

Many things in systems are simple, but the terminology and details makes
them seem scary.  Virtual memory is one of the best examples.  

Mechanically, all we want to do for virtual memory is to map integers
(virtual addresses) to integers (physical addresses).   Most of the
complication in VM boils down to the fact that there is no good way
to construct a general integer function.  If we want to map arbitrary
ints to ints, then we fundamentally have to use a brute force table.
(Quick proof: if the mapping is purely random, then its not compressible,
and we need to record everything.  Also known as a table.)

One question is what granularity the function works on.  In the extreme
we could map every byte in the virtual address space to any byte in
the physical address space.  This is flexible, but very high overhead.
For example, on the ARM, each page table entry (PTE) is 4 bytes, so
the page table would be 4x larger than the address space!  So we do
what we often do in systems to reduce the overhead of memory functions:
restrict flexibility and use quantization.  Here we break the address
space (physical and virtual) into fixed size ranges ("pages").  We allow
any virtual range to be mapped to any physical range, so we still need
a table for that: the page table now maps a virtual page number to a
physical page number.  However, all bytes within a virtual range are
mapped to the byte at the same offset in its associated physical range.
I.e., we use the identity function, which is fast and needs no table.
A 4096 page size will reduce our page table overhead by about 4096x and
because of spatial locality not reduce our flexibility that much.

To make this concrete:  
 - For today's lab, we will just map 1MB regions at a time.  ARM calls
 these "segments".
 - So the page table will map a virtual segment to a physical segment.
 - Our function's Domain: The r/pi has a 32-bit address space, so that
 is 4096 virtual segments.  Thus the function's domain is `[0..4096)`.
 - Our function's Range: Not including GPIO, The r/pi has 512MB of memory,
 so 512 physical segments.  Thus the function's range is `[0..512)`.
 - Thus, we are making a trivial integer function that will map
 `[0...4096) ==> [0..512)`.  (GPIO also adds some numbers to the
 range, but you get the idea.)  You built fancier functions in your
 intro programming class.  The only tricky thing here is that we need
 ours to be very fast.

This mapping (address translation) happens on every instruction, twice
if the instruction is a load or store.  Thus it must be fast.  So as you
expect we'll have one or more caches to keep translations (confusingly
called "translation lookaside buffers").  And, as you can figure out on
your own, if we change the function mapping, these caches have to be updated.
Keeping the contents of a table coherent coherent with a translation cache
is alot of work, so machines generally (always?) punt on this, and it is
up to the implementor to flush any needed cache entries when the mapping
changes. (This flush must either only finish when everything is flushed,
or the implementor must insert a barrier to wait).  

Finally, as a detail, we have to tell the hardware where to find the
translations for each different address space.  Typically there is a
register you store a pointer to the table (or tables) in.

The above is pretty much all we will do:
  1. For each virtual address we want to map to a physical address, insert
  the mapping into the table.
  2. Each time we change a mapping, invalidate any cache affected.
  3. When we start running a process, tell the hardware where to 
  find its translations.

#### ARM-specific reading.

I recommend reading this crash-course in how to do ARM virtual memory:
 * [Concise, concrete pi MMU](https://github.com/naums/raspberrypi/blob/master/mmu/README.md)
Very good place to start.  You can try getting the code to work and play with
it as a great way to remove any unease you have.

The main thing you'll need for this lab: I've gone through about 100
pages of the ARM manual and annotated it.  No guarantee that these
are all the annotations necessary, but they definitely cover stuff you
should understand.  In the `./docs` directory for this lab:
   1. `armv6.b2-memory.annot.pdf`: section B2 of the ARM manual, 
   describing memory ordering requirements ---
   what you have to do when you update the page table, the page table
   registers, etc.

   2. `armv6.b3-coprocessor.annot.pdf`: section B3 of the ARM manual,
   describing the different co-processor options.

   3. `armv6.b4-mmu.annot.pdf`: section B4 of the ARM manual, describing
   the page table format(s), and how to setup/manage hardware state for
   page tables and the TLB.

### General reading:

To re-affirm your grasp of virtual memory, the slides from 
[CS140 lecture notes](http://www.scs.stanford.edu/19wi-cs140/notes/) give
 a big picture overview:
  1. [Virtual memory hardware](http://www.scs.stanford.edu/19wi-cs140/notes/vm_hardware-print.pdf).
  2. [Virtual memory OS](http://www.scs.stanford.edu/19wi-cs140/notes/vm_os-print.pdf)

And for more detail, the book [Operating systems in three easy pieces](http://pages.cs.wisc.edu/~remzi/OSTEP/) provides chapters online.  You want to look at:
  1. [Address spaces](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-intro.pdf).
  2. [Address translation](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-mechanism.pdf).
  3. [Translation lookaside buffers](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-tlbs.pdf).
  4. [Complete VM systems](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-complete.pdf).



### Useful code to compare to

Useful code (use to double-check understanding):
 - [Linux TLB code for V6](https://elixir.bootlin.com/linux/latest/source/arch/arm/mm/tlb-v6.S).
 - [Linux cache code for V6](https://elixir.bootlin.com/linux/latest/source/arch/arm/mm/cache-v6.S).
 - [MMU code for vmwos](https://github.com/deater/vmwos/blob/master/kernel/memory/arm1176-mmu.c)
 - [PiOS source](https://www.stefannaumann.de/git/snaums/PiOS/src/branch/master/source)


