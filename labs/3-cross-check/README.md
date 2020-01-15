Lab: automatically cross-check your pi code against everyone else's.
======================================================================

****THIS IS A DRAFT: will be changed before lab!****

A goal of this course is that you will write every single line of
(interesting) low level code you use.  On the plus side, you will
understand everything and, unlike most OS courses, there will not be
a lot of opaque, magical code that you have no insight into, other than
sense of unease that it does important stuff beyond your ken.  However, an
obvious potential downside of writing the low-level code that everything
depends on: a single bug in it can make the rest of the quarter miserable.

This lab will have you build the initial pieces needed to automatically
check that your pi code is correct.  We will use the approach to check
that both your GPIO and your bootloader implementations  are equivalant
to everyone else's implementation.

The basic idea: After completing the lab you will:

    1. Be able to check that your `gpio` code is equivalant to everyone
    else's in the class by tracing all reads and writes it does and
    comparing them to everyone else's implementation.  If even one person
    gets it right, then showing equivalance means you got it right too.
    And, nicely, automatically detect if any subsequent
	modifications you do break the code (e.g., if you rewrite
    it to be cleaner).

    2. Be able to quickly, correctly write a more full-featured
    `gpio` implementation by checking its equivalance against the
    fairly complicated code used in the initial cs107e class.

#### Sign-off

   1. Show you can run our `test_all` function and get the same trace /
      checksum as everyone else.  (This code checks: `gpio_set_output`,
      `gpio_set_input`, `gpio_set_on`, `gpio_set_off`).

   2. Show you can run our `bootloader_test` and get the same trace /
      checksum as everyone else.

----------------------------------------------------------------------
#### 0. make sure your `gpio.c` can compile on Unix.

In order to make testing easy, we want to be able to run your r/pi code
--- unaltered --- on your Unix laptop.  In general you'll have to do
the following:

  1. Compile it with your Unix compiler (`gcc`).

     Because of how we wrote ` gpio.c` you can easily compile it both
     for RPI and to run on your Unix laptop by simply switching compilers.

  2. Override what happens when it uses ARM assembly code, since your laptop
     (likely) cannot execute this.  

     Fortunately, your GPIO does not use assembly directly; it does call
     the assembly functions `put32` and `get32`, but since we replace them
     (below), this is not an issue.

  3. Emulate reads or writes of GPIO memory addresses --- these will
     likely be illegal on your laptop and will definitely behave
     differently.  (You can check legality by, for example, executing
     `*(volatile unsigned *)0x20200000 = 4` and see if you get a
     segfault!).

     In this case, all you'll have to do is provide fake implementations
     of `get32` and `put32` so that we can override what happens when
     your code writes to GPIO memory.

As discussed in the pre-lab, copy your `gpio.c` into libpi and make sure
it compiles. You'll have to:

  1. Change it to include `rpi.h` rather than `gpio.h` and
     the prototypes it defines (redundant).

  2. Confirm that your GPIO code uses `get32` and `put32`
     instead of a raw read or write of device memory.  So, for example

            *gpio_fsel0 &= ~111b;

     would become:

        put32(gpio_fsel0, get32(gpio_fsel0) & ~111b);

  3. Modify the `put-your-src-here.mk` (the comments describe what to
     do) and run `make` in `libpi`  and make sure everything compiles.

----------------------------------------------------------------------
#### 1. Make a fake implementation of `put32` and `get32`

For this lab and the homework, we need to be able to override what
happens when your code writes to GPIO memory.  Later in this course
(when we build virtual memory), you will use memory protection tricks to
intercept all loads and stores and cause them to do interesting things.
For the moment, we will do things in a much simpler, though manual way.

You'll now build a fake memory so that you can implement `put32` and
`get32`.  First:
  - Look in `fake-put-get.c` and read the comments.  

For `put32(addr,v)`: 
  1. Create an entry for `addr` in your fake memory if `addr` doesn't exist.
  2. Write `v` to `addrs` entry.
  3. Call `print_write(addr,v)`.

On `get32(addr)`:

  1. If `addr` does not exist, insert `(addr, random())` (but do not print anything).
  2. Get the value `v` associated with `addr`.
  3. Call `print_read(addr,v)` to print out the address and value.
  4. Return `v`.

To test it:
  1. Run `./test-put-get > out` on Unix.	It should run without
	   crashing and, importantly, print out the values for each
	   `put32` and `get32` in the exact order they happened.
  2. Get the checksum of the output (`cksum out`) and compare to your partner.
  3. If these values match, you know your code worked the same as your partner's.
  4. Now post to the newsgroup so everyone can compare.
  5. If everyone matches, and one person got it right, we've proven that
       everyone has gotten it right (at least for the values tested).

----------------------------------------------------------------------
#### 3. Check your code against everyone else (5 minutes)

After you checked your fake `put32` and `get32` we now want to check that
your `gpio` code works the same as everyone else.  Given the `get32` and
`put32` modifications above, a simple, stringent approach is to check
that two `gpio` implementations are the same:

  1. They read and write the same addresses in the same order with
  the same values.
  2. They return the same result.    (For our implementations we
  did not return any result, so this just means that your code
  never crashes.)

If both checks pass then we know that both implementations are equivalent
--- at least for the tested inputs.

For this section:
 1. Uncomment out the rule for `test-gpio` in `Makefile` and run `make`.
       This will compile the test harness `test-gpio.c`.
 2. You can test each function individually by running `test-gpio 0`,
       `test-gpio 1`, etc.  (Look in the `test-gpio.c` file.)
 3. Again compare the results to your partner and post to the newsgroup.

----------------------------------------------------------------------
#### 4. Add `gpio_set_func` and cross-check it.

You'll see we can write an implementation even without having a way to run it.

   1.  Implement the `gpio_set_func` function: it already has a prototype 
       in `src/gpio.h`.
   2. Add a test to the test harness.
   3. Make it so that `test-gpio 4` runs it in isolation and `test-gpio 5` runs
      all tests.
   4. Cross-check your results.

----------------------------------------------------------------------
#### 5. Replace our `gpio.o`!

Put it all together:
   1. Follow the pre-lab instructions on how to swap your `gpio.o` for ours.
   2. Compile `3-cross-check/hello` 
   3. Run it on the pi using the bootloader.
   4. Congratulations!  You have removed a big chunk of our code.

#### 5. Advanced.

Making the code more useful mostly involves expanding it so that
even when code writes values in different orders, you can still
show equivalance.

Modify your code to:

    1. Check that your opposing blink code is identical to your
    partners (you will need to make it loop a finite number of times).
    What should you do about `delay`? 

    2.  Show that its equivalant even when you set output pins
    in different order.  Hint: you likely want to be able
    easily mark some memory as "last writer wins" where it doesn't
    matter the actual order, just the final value.   I would
    do this by tracing each function, automatically marking the
    addresses it writes as last-writer addresses.

    3. How to show equivalance when you set and clear in different orders?
    This is trickier.  You will need to come up with a clean scheme to
    indicate that non-overlapping bit-writes do not interfere.

    4. Within a device, often you can write some fields in any order, and
    there is a final "commit" location you write to turn the device on.
    Devise a good way to cleanly indicate these differences.

    5. Strictly speaking, we need memory barriers when writing to
    different devices.  Add support for checking this.

If you can do all of these you are in great shape to check some
interesting code.
