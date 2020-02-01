### Interrupts.

Big picture for today's lab:

   0. We'll show how to setup interrupts / exceptions on the r/pi.
   Interrupts and exceptions are useful for responding quickly to devices,
   doing pre-emptive threads, handling protection faults, and other things.

   1. We strip interrupts down to a fairly small amount of code.  You will
   go over each line and get a feel for what is going on.  

   2. You will use timer interrupts to implement a simple but useful
   statistical profiler similar to `gprof`.  As is often the case,
   because the r/pi system is so simple, so too is the code we need to
   write, especially as compared to on a modern OS.

The good thing about the lab is that interrupts are often made very
complicated or just discussed so abstractly it's hard to understand them.
Hopefully by the end of today you'll have a reasonable grasp of at least
one concrete implementation.  If you start kicking its tires and replacing
different pieces with equivalant methods you should get a pretty firm
grasp.

#### Why interrupts

As you'll see over the rest of the quarter, managing multiple devices
can be difficult.  Either you check constantly (poll), which means most
checks are fruitless.    Or you do not check constantly, which means most
actions have significant delay since the device has stuff for you to do
much before you check it.  Interrupts will allow you do to your normal
actions, yet get interrupted as soon as an interesting event happens on
a device you care about.

You can use interrupts to mitigate the problem of device input by telling
the pi to jump to an interrupt handler (a piece of code with some special
rules) when something interesting happens.  An interrupt will interrupt
your normal code, run, finish, and jump back.  

Interrupts also allow you to not trust code to complete promptly, by giving
you the ability to run it for a given amount, and then interrupt
(pre-empt) it and switch to another thread of control.  Operating systems
and runtime systems use this ability to make a pre-emptive threads
package and, later, user-level processes.  The timer interrupt we do
for today's lab will give you the basic framework to do this.

Traditionally interrupts are used to refer to transfers of control
initiated by "external" events (such as a device or timer-expiration).
These are sometimes called asynchronous events.  Exceptions are
more-or-less the same, except they are synchronous events triggered by the
currently running thread (often by the previous or current instruction,
such as a access fault, divide by zero, illegal instruction, etc.
The framework we use will handle these as well; and, in fact, on the
arm at a mechanical level there is little difference.

Like everything good, interrupts also have a cost:  this will be our
first exposure to race conditions.  If both your regular code and the
interrupt handler read / write the same variables (which for us will be
the common case) you can have race conditions where they both overwrite
each other's values (partial solution: have one reader and one writer)
or miss updates because the compiler doesn't know about interrupt handling
and so eliminates variable reads b/c it doesn't see anyone changing them
(partial solution: mark shared variables as `volatile`).  We'll implement
different ways to mitigate these problems over the next couple of weeks.

### Deliverables:

Turn-in:

  1.  Look through the code in `timer-int`, compile it, run it.  Make sure
  you can answer the questions in the comments.  We'll walk through it
  in lab.

  2. Implement `gprof` (in the `gprof` subdirectory).   You should run it
  and show that the results are reasonable.

### Using interrupts to build a profiler.

The nice thing about doing everything from scratch is that simple things are simple
to do.  We don't have to fight a big OS that can't get out of its own way.   

Today's lab is a good example: implementing a statistical profiler.  The basic intuition:
   1. Setup timer interrupts so that we get them fairly often.
   2. At each interrupt, record which location in the code we interrupted.  (I.e., where
      the program counter is.)
   3. Over time, we will interrupt where your code spends most of its time more often.

The implementation will take about 30-40 lines of code in total.  You'll build two things:
 1. A `kmalloc` that will allocate memory.  We will not have a `free`, so `kmalloc` is
   trivial: have a pointer to where "free memory" starts, and increment a counter based
   on the requested size.
 2. Use `kmalloc` to allocate an array at least as big as the code.
 3. In the interrupt handler, use the program counter value to index into this array
    and increment the associated count.  NOTE: its very easy to mess up sizes.  Each
    instruction is 4 bytes, so you'll divide the `pc` by 4.  
 4. Periodically you can print out the non-zero values in this array along with the 
   `pc` value they correspond to.  You should be able to look in the disassembled code
   (`gprof.list`) to see which instruction these correspond to.

Congratulations!  You've built something that not many people in the Gates building
know how to do.

### lab extensions:

There's lots of other things to do:

  1. Mess with the timer period to get it as short as possible.

  2. To access the banked registers can use load/store multiple (ldm, stm)
    with the caret after the register list: `stmia sp, {sp, lr}^`.

  3. Using a suffix for the `CPSR` and `SPSR` (e.g., `SPSR_cxsf`) to 
	specify the fields you are [modifying]
     (https://www.raspberrypi.org/forums/viewtopic.php?t=139856).

  4. New instructions `cps`, `srs`, `rfe` to manipulate privileged state
   (e.g., Section A4-113 in `docs/armv6.pdf`).   So you can do things like

            cpsie if @ Enable irq and fiq interrupts

            cpsid if @ ...and disable them

### Supplemental documents

There's plenty to read, all put in the `docs` directory in this lab:
 
  1. If you get confused, the overview at `valvers` was useful: (http://www.valvers.com/open-software/raspberry-pi/step04-bare-metal-programming-in-c-pt4)

  2. We annotated the Broadcom discussion of general interrupts and
  timer interrupts on the pi in `7-interrupts/docs/BCM2835-ARM-timer-int.annot.pdf`.
  It's actually not too bad.

  3. We annotated the ARM discussion of registers and interrupts in
  `7-interrupts/docs/armv6-interrupts.annot.pdf`.

  4. There is also the RealView developer tool document, which has 
  some useful worked out examples (including how to switch contexts
  and grab the banked registers): `7-interrupts/docs/DUI0203.pdf`.

  5. There are two useful lectures on the ARM instruction set.
  Kind of dry, but easier to get through than the arm documents:
  `7-interrupts/docs/Arm_EE382N_4.pdf` and `7-interrupts/docs/Lecture8.pdf`.

If you find other documents that are more helpful, let us know!
