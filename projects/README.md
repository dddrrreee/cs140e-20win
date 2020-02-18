## Some final projecct possibilities.

Final projects:
  - Do something interesting!
  - 1-4 people: the more people, the more you better do.
  - You can spend $40 or so without asking (we'll reimburase) but if you
  need more check first.
  - Presentations will be during the final exam slot.  

You can always devise your own final project but I've typed out some
possible options. 

### Do more OS

We've done a bunch of hello-world level hacks: you could build them into
something more full-features:

  - Add networking to the pi.  One option is to hook it up to the 
  [esp9266](https://www.sparkfun.com/products/13678) via the uart and
  communicate that way.  Another is to use xbee's.

  - Develop the virtual memory system into one that is full featured, able
  to handle the different page sizes, the different protection bits, etc.
  Have it correctly catch when code references beyond the end of the heap,
  dereferences null, and needs to dynamically grow the stack.

  - Do a simple `FAT32` file system so sensors can write data to the sd
  card and keep it around; make this available using FUSE so your laptop
  can access.  Cooler: use the wireless above to export the SD's FAT32
  file system to your computer remotely using FUSE.

  - We use the `r/pi A`+, put there are many little boards out there.
    An interesting project is porting a bunch of the different code
    we've built to another system.

    I have the following boards available:
    - [Teensy 2.8, 3.2](https://www.sparkfun.com/products/13736): fast, small
    - [pocketbeagle](https://beagleboard.org/pocket): smaller than the pi!
    - various [esp8266 boards](https://www.sparkfun.com/products/13678): 
    a low-cost, wifi capable system.
    - [pi zero](https://www.adafruit.com/product/2885): a smaller pi. 

  - Assemble all the different pieces you've built into a complete, 
    small, embedded OS.   I'd suggest domains, threads, some support 
    for deadlines, and channels (pipes) to communicate.

  - Extend your FUSE file system into a full-fledged way to interact
    with the pi.  Have a method to mount devices into the file system,
    push results to the device and back, and in general have a way to
    share traffic over the UART.  You will also certainly have to add
    interrupts to handle the UART (recall it has an 8-byte queue for
    transmit and receive, so can easily drop bytes) as well as having a
    more efficient approach to sending communication back/forth between
    your laptop and pi.

  - Make a clean system that can sensibly blend pre-emptive, cooperative,
    and deadline-based run-to-completion threads (which do not need
    context switching)

  - Rewrite the interrupt / exception handling to be much more efficient
    and extensible.  Rip the code down to the bare minimum, enable icache,
    dcache, BTB, any other tuning you can.  See how much faster you can
    make it compared to where we started.  
    
    Take micro-benchmarks from the literature and see how much you can
    beat them by (how fast you can ping-pong bytes between threads,
    take a protection fault, etc).  If you beat linux/macos by 50x I
    wouldn't be surprised.

### Build a Tool

  - Volatile cross-checking.  A very common, nasty problem in embedded
    is that the code uses pointers to manipulate device memory, but
    either the programmer does not use `volatile` correctly or the
    compiler has a bug.  We can detect such things with a simple hack:

    - We know that device references should remain the same no matter 
      how the code is compiled.  
    - So compile a piece of code multiple ways: with no optimization, `-O`,
    `-O2`, with fancier flags, etc.  
    - Then run each different version, using ARM domain tricks to trace 
      all address / values that are read and written.  
    - Compare these: any difference signals a bug.  

    This is basically your second lab, with some high-end tricks.
    It would have caught many errors we made when designing cs107e;
    some of them took days to track down.

  - Build a debugger that can run over the UART.  Insert breakpoints to
  stop execution.  Use the special ARM hardware to do data watch-points.
  Figure out how to do a backtrace and to match up instruction program
  counter values to the C code (not hard if you use the `.list` files).  
  You'll likely have to add interrupts to the UART.

  - Do a trap-based valgrind/purify so you can detect memory corruption.
    Valgrind checks every load and store to see if you are writing outside
    of an object's boundary.  It does so by dynamically rewriting the
    executable code.  This is hard.  Instead you can use your virtual
    memory system to:
    - Mark all heap memory as unavailable.
    - In the trap handler, determine if the faulting address is in bounds.
    - If so: do the load or store and return.
    - If not: give an error.
  
    Given how fast our traps are, and how slow valgrind is, your approach
    might actually be faster.

  - Do a trap-based race detector: similar to valgrind above, Eraser
  is a well known (but dead) tool for finding race conditions that worked
  by instrumenting every load and store and flagging if a thread accessed 
  the memory with inconsistent set of locks.  As above, b/c binary rewriting 
  is hard you can use memory traps to catch each load/store and check if
  the locks the current thread holds are consistent.

  - Do a statistical version of either the race detector or memory
  checker above: set your timing interrupts to be very frequent and
  in the handler, do the check above.  It may miss errors, but will be
  very fast and should do a reasonable job, given a long enough run and
  a fine-enough window.

  - Write cooperative thread checkers that detect when you run too long
  with interrupts disabled, too long without yielding, in a deadlock,
  spinning on a condition that cannot change, extend past the end of
  the stack, have priority inversion, starvation, missed deadlines,
  lock queues that are too long, or critical sections that are too long.

  - Extend your bootloader checker to check other network / distributed
  systems protocols.  (This is likely only feasible if you've taken a
  distributed system or networking clas.)   These protocols do not get
  exhaustively tested for every possible failure, so often have subtle flaws.
  You're in a good position to find some.

  - ARM code is not that hard to parse (at least compared to x86).  We can
    use this ability to make a effective, but hopefully simple lock-free
    algorithm checker.   Given a set of functions that purport to run
    correctly (either with or without locks) we can:
    - disassemble them.
    - at each load or store, insert a call to a context switch.
    - run the code with two threads.   
    - first test that they give the same result as two sequential calls
      if we do a single context switch at each possible point, then two 
      context switches, etc.

    To be really fancy, we can buffer up the stores that are done and then
    try all possible legal orders of them when the other thread does a load.
    This checker should find a lot of bugs.

### Stupid domain tricks

As discusse lab, you can use ARM domains to very quickly switch the
permissions for a set of pages.    It'd be interesting to use this
ability for various things.  One possibility: use it to make embedded
development less dangerous.  

##### Protected subsystems.

As you probably noticed, we have no protection against memory corruption.
As your system gets bigger, it becomes much much harder to track down which
subsystem is causing everything to crash.  Going all the way to 
UNIX style processes is one attack on this problem, but we can use
ARM do to a lighter weight method:
   - give each domain a unique domain number.
   - tag all memory associated with the domain with its domain number.
   (you may have to have additional domain numbers for pages accessible
   by more than one).
   - before you call into the domain, change the domain register so that
   domain has client privileges, but no other domain is accessible.
   - at this point it can only touch its own memory.

This method is much faster than switching between processes: we don't
have to flush the TLB, caches, or page table.

###### Lightweight protection domains

As a lighter-weight intermediate hack: if we want to proect one thread
from another, we usually throw it in a different address space.  This can
be a lot of overhead.  Instad, tag its memory with a seperate domain ID
and do what we do above.

###### Subpage-protection

If we have some code that is causing corruption but we can't fiure out,
use domains to quickly turn off/on write-permissions for all memory
(or for unrelated memory) and monitor the traps that occur to see if
the code is accessing outside of its region.
