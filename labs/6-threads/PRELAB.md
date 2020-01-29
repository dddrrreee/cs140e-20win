## Prelab for threads

In this lab, you'll write a simple non-preemptive thread package.

We'll have to save and restore registers for thread context switching and know
how procedure calls work at the assembly level:
  1. `6-threads/docs/subroutines.hohl-arm-asm.pdf` has a reasonable overview of both.  It's
    written for assembly-programmers so you should skim to where there are annotated
    places.
  2. For ARM assembly code, the [ARM asm talk](docs/Arm_EE382N_4.pdf) in the `6-threads/doc`
     directory is a reasonable crash course.
  3. The ARM manuals in our top-level `docs` directory have the definitive,
     detailed description of anything you need to know.

Some useful introduction (or review) reading for threads:
  1. [Lecture 2 and Lecture 3](https://www.scs.stanford.edu/18wi-cs140/notes/) from Mazieres'
     CS140 lectures.
  2. [Threads intro](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-intro.pdf)
     and [Thread API](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-api.pdf)
     from the useful [Three Easy Pieces](http://pages.cs.wisc.edu/~remzi/OSTEP/#book-chapters)
     textbook.

Deliverables:
  1. Implement kernel memory allocation in `1-kmalloc` (see below).  
  2. Look through the `2-threads/rpi-thread.[ch]` header and skeleton code
     so you see what you will be implementing.
  3. Read about threads carefully, especially the API chapter above.  
  4. Figure out these puzzles: what do we have to do to context
     switch from one thread to another?  How to switch from "normal
     execution" to the first thread?  How to handle the case where a
     thread running `foo()` does not call `rpi_thread_exit` explicitly
     and simply returns back to its (non-existant) caller?

-------------------------------------------------------------------------------
###  Implement `kmalloc` and `kmalloc_init` in `kmalloc.c`.

In today's lab we want to create an arbitrary number of threads and
stacks, so we would like a kernel memory allocator.   To let us focus on
threads we are are just going to do a trivial one that does not support
free's of individual blocks.  The great thing about memory allocation
without free is that it is trivial: simply increment a pointer!  (Note,
we do have a free of the entire memory space: it's called `rpi_reboot`).

The first problem we have is figuring out where the heap starts.
We will use a simple hack: just put it at the end of the pi program:
  - If you look at the linker script `libpi/memmap`, you can see that it
    tells the linker to place a symbol `__heap_start__` at the end of
    the executable so we can locate it.  The address of this variable
    is the address of the start of the heap.  

The second problem we have is that when we allocate, we have to make
sure that the memory returned can be accessed without causing an
alignment fault.  Since the pi only requires 4-byte alignment, we can
just make sure every returned block is at least 4-byte aligned (i.e.,
the lower two bits should be 0).

Your implementation should do the following:
   1. In `kmalloc_init` set the heap to be the address `__heap_start__`
      is located at.  Make sure it is 4-byte aligned.  Print it to make
      sure the address makes sense.

   2. In `kmalloc(nbytes)` roundup `nbytes` to be a 
      multiple of four (use the `roundup` macro) and increment
      the heap pointer by the result so that the next allocation
      occurs afterwards.  Assert that the result makes sense.

When you are done:
   1. Compile and run `test-kmalloc`; feel free to add a bunch of tests.  
     I did a bare minimum.  It checks that you roundup alignment 
     and allocate no more than is needed.
  2. If it passes: . Copy your implementation to `libpi/my-src`.
  3. Check that `2-threads` (which uses `kmalloc`)
     compiles without a link error.  If you get one you probably didn't
     copy into `libpi/my-src`.

Various further readings:
  - A reasonable article on [linker scripts](https://interrupt.memfault.com/blog/how-to-write-linker-scripts-for-firmware)
  - [Regon allocation] (https://en.wikipedia.org/wiki/Region-based_memory_management).
  - [Arena allocation] (http://drhanson.s3.amazonaws.com/storage/documents/fastalloc.pdf).
