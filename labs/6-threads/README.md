## Writing a non-preemptive threads package

***MAKE SURE YOU DO THE [PRELAB](PRELAB.md) FIRST!***

Big picture:  by the end of this lab you will have a very simple
round-robin threading package for "cooperative" (i.e., non-preemptive)
threads.  

The main operations (see `rpi-thread.[ch]`):
  1. `rpi_fork(code, arg)`: to create a new thread, put it on the `runq`.
  2. `rpi_yield()`: yield control to another thread.
  3. `rpi_exit(int)`: kills the current thread.
  4. `rpi_thread_start()`: starts the threading system for the first time.
      Returns when there are threads left to run.

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
### Hints

While all this is exciting, a major sad is that a single bug can lead
to your code jumping off into hyperspace.  This is hard to debug.
So before you write a bunch of code:
  1. Try to make it into small, simple, testable pieces.
  2. Print all sorts of stuff so you can sanity check!  (e.g., the value
  of the stack pointer, the value of the register you just loaded).
  Don't be afraid to call C code from assembly to do so.

The single biggest obstacle I've seen people make when writing assembly is
that when they get stuck, they wind up staring passively at instructions
for a long time, trying to discern what it does or why it is behaving
"weird."

  - Instead of treating the code passively, be aggressive: your computer
    cannot fight back, and if you ask questions in the form of code
    it must answer.  When you get unsure of something, write small
    assembly routines to clarify.  For example, can't figure out why
    `LDMIA` isn't working?  Write a routine that uses it to store a
    single register.  Can't figure out if the stack grows up or down?
    take addresses of local variables in the callstack print them (or
    look in the `*.list` files).

   - Being active will speed up things, and give you a way to
    ferret out the answer to corner case questions you have.

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

   - `r13` : stack pointer (`sp`).  Today it doesn't matter, but in the 
      general case where you save registers onto the sack,
      you will be using the stack pointer `sp`.  Be careful when you save
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

  - `r4 --- r12, r13, r14`.    Note: if you use `LDM` and `STM`
      instructions,
      they cannot have the stack pointer in the register list (it can
      be used as the base).

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
### Part 1: getting run-to-completion threads working (20 minutes)

I've tried to break down the lab into isolated pieces so you can test
each one.  The first is to get a general feel for the `rpi_thread`
interfaces by getting "run-to-completion" (RTC) threads working: these
execute to completion without blocking.

RTC threads are an important special case that cover a surprising
number of cases.  Since they don't block, you can run them 
as deferred function calls.   They should be significantly faster
than normal threads since:
  1. you don't need to context switch into them.
  2. you don't need to context switch out of them.
  3. you won't have to fight bugs from assembly coding.
Most interrupt handlers will fall into this category.

What to change:
  1. `rpi_exit` just return immediately (not correct in general, but ok for
     our test).
  2. `rpi_yield` should just return immediately.
  3. `rpi_fork` create a thread and put it in the run-queue.
  4. `rpi_thread_start` remove threads in FIFO order, run each function, free the 
     thread block.

With these changes, `1-test-thread` should complete successfully.

----------------------------------------------------------------------
### Part 2: assembly and saving state (30 minutes)

Our next step is to start writing assembly code and build up the 
context-switching code in pieces.  

##### Step 0: hello world in assembly

To get started, write a simple assembly routine `store_one_asm(p,u)`
in `test-asm.S` that stores `u` into `*p`.    When you run `2-test`
the first test should work (the second will fail until the next part). 

##### Step 1: save registers

Next, we build a routine that correctly saves callee-saved registers.
If you look in `test-asm.S` at `check_callee_save`:
  1. Write the code to load a known constant into each register --- with
     the exception of the stack pointer --- that you will
     save so you can check their saved values.
  2. Write the code to store each callee-saved register into the given block of memory
     in ascending order register number.
  3. The code then 
     calls `print_and_die` with a pointer to the block as the first
     argument.

When you run the test, each printed register other than the stack pointer
should match its expected value (the location for `r4` should hold
`4` etc).


Note that the bulk register save and restore instructions store the smallest
registers so that the smallest is at the lowest address; also, don't 
include the stack pointer in the register list!   Useful [ARM
   doc](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0473m/dom1359731152499.html)
   or [this](https://www.heyrick.co.uk/armwiki/STM).

Don't be afraid to go through the ARM manual (`docs/armv6.pdf`) or the
lectures we posted in `6-threads/docs`.

----------------------------------------------------------------------
### Part 3: context-switching.

Now that you can save registers, it makes sense to restore them.
Using the starter code in `test-asm.S`
build a context switch routine `cswitch(uint_t *old, uint_t *new)` that:
  1. Saves the callee-saved registers into the block pointed to by `old` (as
     above).
  2. Loads the callee-saved registers previously saved into the block
     pointed to by `new`.
  3. Jumps back to the address in `lr`.

Testing: `3-test-cswitch` has two parts:
  1. Checks that inserting calls that save and restore from the same
     block (e.g., calling  `cswitch(p,p)`) does not change the behavior of code.
  2. Check that reloading from a previously saved block the code to jump backwards
     and re-execute as expected.  (How does this test work?)

----------------------------------------------------------------------
### Part 4: Make a full threads package.

Now we make your RTC threads work with context switching.
Make the following changes:


  - `rpi_start_thread` will context switch into the first thread it
     removes from the run queue.  Doing so will require creating a
     dummy thread so that when there are no more runnable threads,
     `rpi_cswitch` will transfer control back and we can return back
     to the main program.  (If you run into problems, try first just
     rebooting when there are no runnable threads.)

  - `rpi_fork` should setup the thread's register save area so that an
     initial context switch will work.  (Discussed more below.)

  - `rpi_exit` if it can dequeue a runnable thread, context switch
     into it.  Otherwise resume the initial start thread created in
     step 1.

  - Move your `cswitch` code into `rpi_cswitch` in `thread-asm.S`

     Make sure you verify that your `rpi_cswitch` code works when you
     pass the current thread as both arguments --- behaviorally this
     is a no-op since your code should save and restore the state,
     and resume right after the call.

  - Change `rpi_yield` so that it works as expected.


Given that you have context-switching, the main tricky thing is figuring
out how to setup a newly created thread so that when you run context
switching on it, the right thing will happen (i.e., it will invoke to
`code(arg)`).

   - The standard way to do this is by manually storing values
     onto the thread's stack (sometimes called "brain-surgery") so that
     when its state is loaded via `rpi_cswitch` control will jump to
     a trampoline routine (written in assembly) with `code` with `arg`
     in known registers The trampoline will then branch-and-link to the
     `code` with `arg` in `r0`.

   - The use of a trampoline lets us handle the problem of missing
     `rpi_exit` calls.  The problem: If the thread code does not call
     `rpi_exit` explicitly but instead returns, the value in the `lr`
     register that it jumps to will be nonsense.  Our hack: have our
     trampoline that calls the thread code simply call `rpi_exit` if the
     intial call to `code` returns.

   - To help debug problems: you can initially have the
     trampoline code you write (`rpi_init_trampoline`) initially just
     call out to C code to print out its values so you can sanity check
     that they make sense.

Checking:
   1. When you run `3-test` it should work and print `SUCCESS`.
   2. You should also be able to run `1-test` now that your yield and exit are not
      broken.
   3. Finally you should be able to run two LEDs in `4-test-yield`.

Congratulations!  Now you have a simple, but working thread implementation
and understand the most tricky part of the code (context-switching)
at a level most CS people do not.

-------------------------------------------------------------------------
### Lab extensions

There's a lot more you can do.  We will be doing a bunch of this later
in the class:

  - Save state onto the stack itself.

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
