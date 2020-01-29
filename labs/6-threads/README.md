## Writing a non-preemptive threads package

***MAKE SURE YOU DO THE [PRELAB](PRELAB.md) FIRST!***

Big picture:  by the end of this lab you will have a very simple
round-robin threading package for "cooperative" (i.e., non-preemptive)
threads.

Recall we split threads into two types:

  - Non-preemptive threads run until they 
    explicitly yield the processor.

  - Pre-emptive threads can be interrupted at any time (e.g.,
    if their time slice expires, or a higher-priority thread becomes
    runnable, etc).

The trade-offs:

  - Cooperative threads make preserving large invariants easy: by
    default, all code is a critical section, only broken up when you
    yield [Atul et al].  Their downside is that if they don't yield
    "often enough" they add large, difficult-to-debug latencies.

  - Pre-emptive threads let you to eliminate the need to trust
    that the threading code yields control in a well-behaved way, which
    is why   multi-user OSes (Linux and MacOS) preempt user processes.
    However, they make it much more difficult to write correct code.
    (E.g., would your gpio implementation work if it was called by
    multiple pre-emptive threads?  Uart?)

Generally, at a minimum, each thread has its own stack --- which has
to be "large enough" so that it doesn't get overrun --- and the thread
package has to provide an assembly routine to "context switch" from one
thread to another.  Context switching works by:

   1. Saving all registers from the first thread onto its stack (or as we do,
      into the thread structure itself).
   2. Loading all registers for the second from where they were saved.
   3. Changing the program counter value to resume the new thread.

The main magic you'll do today is writing this context-switching code.
Context-switching doesn't require much work --- less than 15 instructions
using special ARM instructions, but mistakes can be extremely hard
to find.  So we'll break it down into several smaller pieces.

### Sign-off:

   1. You have a working `kmalloc` implementation.
   2. You pass the tests in the 2-threads` directory.

----------------------------------------------------------------------
### Background: A crash course in ARM registers

You should have read the ARM assembly chapter in the PRELAB
(`docs/subroutines.hohl-arm-asm.pdf`); this is
just a cliff-notes version.

Context-switching may sound mysterious, but mechanically it is simple: we
just need to need to save all the registers in use.  The ARM has sixteen
general-purpose registers `r0`---`r15` which can be saved and restored
using standard load and store instructions.  If you look through the
`.list` files in any of the pi program directories, you'll see lots of
examples of saving and loading registers.

Even easier, because we are doing non-preemptive threads we don't have to
save all the registers, just the "callee saved" ones.  The ARM, like most
modern architectures, splits registers in into two categories (Q: why?):

  - "Callee-saved": must be saved by a procedure before any use and
    then restored before returning to the calling routine.

  - "Caller-saved": can be used as scratch registers by any procedure
    without saving/restoring them first.  The downside is that if a
    routine needs the contents of a caller-saved register preserved
    across a function call it must save the register before the call
    and restore it afterwards.

As we mentioned, a non-preemptive thread only context switches if it
explicitly calls a thread yield procedure of some kind (in our case,
`rpi_yield()`).  As a result we only need to save the "callee-saved"
registers, since any live caller-saved registers must be already saved
by the compiler.

The ARM general-purpose registers:
   - `r0`---`r3`: caller-saved argument registers.
   - `r4 --- r11`: callee-saved.
   - `r12` may also be caller-saved, depending on the compiler,
      but I can't find a definitive statement for `arm-none-eabi-gcc`, so
      we will treat it, and all the rest as callee-saved: i.e., they must
      be saved before we can use their contents since the caller assumes
      their values will be preserved across calls.  

   - `r13` : stack pointer (`sp`).  While you are saving state, you
      will be using the stack pointer `sp`.  Be careful when you save
      this register.  (How to tell if the stack grows up or down?)

   - `r14` : link register (`lr`), holds the address of the instruction
     after the call instruction that called the current routine (in our
     case `rpi_yield`).  Since any subsequent call will overwrite this
     register we need to save it during context switching.

   - `r15`: program counter (`pc`).  Writing a value `val` to `pc` causes
     in control to immediately jump to `val`.  (Irrespective of whether
     `addr` points to valid code or not.)  From above: moving `lr` to `pc`
     will return from a call.  Oddly, we do not have to save this value!

So, to summarize, context-switching must save:

    - `r4 --- r12, r13, r14`.  

For reference, the [ARM procedure call ABI](http://infocenter.arm.com/help/topic/com.arm.doc.ihi0042f/IHI0042F_aapcs.pdf) document:
<table><tr><td>
  <img src="images/arm-registers2.png"/>
</td></tr></table>


The link [here](https://azeria-labs.com/arm-data-types-and-registers-part-2/) has a more informal rundown of the above.

You can find a bunch of of information in the ARM Manual:
<table><tr><td>
  <img src="images/arm-registers.png"/>
</td></tr></table>

----------------------------------------------------------------------
### Part 0: using assembly to validate your understanding (30 minutes)

This first part makes sure you have the basic tools to validate your
understanding of assembly code by having you write a few assembly
routines.  Being comfortable doing so will come in handy later.

   1. Implement a simple assembly routine `test_csave(p,...)` that that
   stores *all* general-purpose registers in the pointed-to memory
   `p` using the ARM instruction `str` and explicit constant offsets
   (i.e., there will be one instruction for each register saved).  Verify
   that the values stored and the amount of space it used makes sense.

   2. Write another version `test_csave_stmfd` using ARM's
   "store multiple", (should be just a few lines)  and verify
   you get the same values as (2).  Note that `stmfd` stores
   registers so that the smallest is at the lowest address.   Useful [ARM
   doc](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0473m/dom1359731152499.html)
   or [this](https://www.heyrick.co.uk/armwiki/STM).

Don't be afraid to go through the ARM manual (`docs/armv6.pdf`) or the
lectures we posted in `6-threads/docs`.

----------------------------------------------------------------------
### Part 1: Cooperative context-switching (20 minutes)

Context switching will involve inverting the code you wrote for Part 0.

  1. Implement `rpi_cswitch(cur,next)` (see `rpi-thread.h`), which saves
  the current registers to `cur`, and loads all the values from `next`.

  2. To make your code easy to test, make sure your `rpi_cswitch`
  code works when you pass the current thread as both arguments ---
  behaviorally this is a no-op since your code should save and restore
  the state, and resume right after the call.

----------------------------------------------------------------------
### Part 2: Make simple threads (60 minutes)

***NOTE: rewriting this part and the thread test code***

Congratulations!  You can now build a simple threading system.

Implement:

  1. `rpi_fork(code, arg)`: to create a new thread, put it on the `runq`.


  2. `rpi_yield()`: yield control to another thread.

  3. `rpi_exit(int)`: kills the current thread.

  4. `rpi_thread_start()`: starts the threading system.

Given that you have context-switching, the main tricky thing is figuring
out how to setup a thread for the first time so that when you run context
switching on it, the right thing will happen (i.e., it will invoke to
`code(arg)`).  The standard way to do this is by manually storing values
onto the thread's stack (sometimes called "brain-surgery") so that when
its state is loaded via `rpi_cswitch` control will jump to a trampoline
routine (written in assembly) with `code` with `arg` in known registers
The trampoline will then branch-and-link to the code with `arg` in
`r0`.  We use a trampoline so that if `code` returns, we can then call
`rpi_thread_exit()`.

While all this is exciting, a major sad is that a single bug can lead
to your code jumping off into hyperspace.  This is hard to debug.
So before you write a bunch of code:

  1. Try to make it into small, simple, testable pieces.

  2. Print all sorts of stuff so you can sanity check!  (e.g., the value
  of the stack pointer, the value of the register you just loaded).
  Don't be afraid to call C code from assembly to do so.

To break down the pieces:

  0. Initially: Have `rpi_thread_start()` just reboot when there are no
  more threads.

  1. Have the trampoline code you write (`rpi_init_trampoline`) initially
  just call out to C code to print out its values so you can sanity check
  that they make sense.

  2. Then only create a single thread and make sure it can run and `rpi_exit`
  explicitly.

  3. Then make sure if it doesn't exit it will do so implicitly.

  4. Then sure it can `rpi_yield` to itself.

  5. Then change `rpi_thread_start()` to create a dummy thread so that
  when there are no more runnable threads, `rpi_cswitch` will transfer
  control back and we can return to its callsite in `part2`.

Congratulations!  Now you have a simple, but working thread implementation
and understand the most tricky part of the code (context-switching)
at a level most CS people do not.

-------------------------------------------------------------------------
### Lab extensions

There's a lot more you can do.  We will be doing a bunch of this later
in the class:

  - save state onto the stack itself.

  - Make versions of the `libpi` `delay` routines that use your function rather than
  busy wait.  Make sure to check if threads are enabled in `rpi_yield()`!

  - Have a `sleep_until_us(us)` to put the current thread to sleep with a 
  somewhat hard-deadline that it is woken up in `usec` micro-seconds.  Keep
  track of your cumulative error to see how well you're scheduling is doing.

  - Tune your threads package to be fast.  In particular, you can avoid
  context switching when a thread is run for the first time (there is
  no context to load) and you don't thave to save state when it exits
  (it's done).  You can also try tuning the interrupt handling code,
  since it is at least 2x slower than optimal.  (I'm embarrassed, but
  time = short.  Next year!)

   - See how many PWM threads you can run in `3-yield-test`.
