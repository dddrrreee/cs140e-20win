---------------------------------------------------------------------------------
## `libpi-fake`: simple user-space testing library for the pi


#### Caveat.

Currently the discussion below is much too mechanical, and completely
ignores fundamental problems in emulation and equivalence checking, such as: 

   1. When you read pi hardware state, how do you know what to do?
      In some cases writes are stable, and a read returns the last
      value written.  In others, the hardware could return a different
      value each time, depending on the environment and its configuration.
      Knowing a good (much less correct) value to return can be difficult.
      Fortunately, we are not trying to run pi programs with perfect
      fidelity so that they produce the same output they would on the pi
      --- we are just trying to detect when they compute the same answer
      (versus the right answer).


   2. When different methods have the same result, what is
      a good approach to showing equivalence?  For example, one program
      sets GPIO pin 19, then 20 to output pins, while another sets 20
      then 19 --- same result, but our current equivalence checking
      would flag these as different.

Hopefully this can get filled in later.

------------------------------------------------------------------
### Overview

This library will build up additional code we need to run pi programs
on a fake Unix environment for testing.  I've put some initial simple
starter code as an example of one way to do this.  You will be extending
and modifying this as the quarter goes on.

Because embedded systems are so bare-metal they can be the hardest to
debug; on the other hand, because they are so bare-bones, they can be
the easiest to emulate and check much more aggressively than you could,
for example, 30 million lines of Linux OS source.

Our method of cross-checking has two main parts:

  1. Getting your pi code to run on Unix on your laptop.  This involves
     replacing different pi functions with our own versions.

  2. Tracing what your pi code did.  For the most part, we simply print
     out each pi routine called (possibly with extra information).

     If your code *does not* visit the same routines in the same order
     as someone else then it's probable (but not guaranteed!) that
     does not compute the same results.   Of course, since we do not
     perfectly simulate the pi, trace equivalence cannot guarantee
     actual equivalences (much less correctness), however it is often
     significantly stronger than just running a few tests and saying
     "ship it".  The `driver.c` code can do as many random runs as you
     like, which can somewhat mitigate this problem.

An additional benefit of running tests on Unix is that it has memory
protection, so if you do an invalid memory access there is some
chance you will detect this with a crash.  We do not get anything
similar on the pi for a few more weeks --- our pi setup will not
even catch a dereference of null now, as you can check:

    // trivial program to run on the pi to show no memory protection.
    volatile unsigned *null = (void*)0;

    *null = 0xfeedface;
    printk("*NULL = %x\n", *null);

    // should be same
    printk("*NULL = %x\n", GET32(0));

------------------------------------------------------------------
###  `libpi-fake` organization

Different fake pieces of `libpi` are in their associated files:

  - `fake-gpio.c`: each called function prints it was called and with 
    what pin.  `gpio_read` randomly returns 0 or 1.  You can bias the
    distribution.

  - `fake-printk.c`: simple `printk` that just calls `printf`.

  - `fake-put-get.c`: your code.

  - `fake-reboot.c`: calls exit.

  - `fake-time.c`: simple version of `time_get_usec` that advances the
    current time by 1 micro-second each time it's called.  Also the `delay_ms`
    and `delay_usec` functions that similarly advance the time.

  - `driver.c`: a driver to call a pi programs `notmain`.

Support code:
  
  - `pi-random.c`: a ripped off version of `random` found in most
  UNIX `libc` implementations.  While the fact that `random` returns
  a pseudo-random number is defined in the standard, its the exact
  implementation is not.  Since our testing code uses `random()`
  extensively, differing versions would lead to spuriously different
  comparisons.  Thus, we include our own version. 

Code changes:
  1. Since some people had `Makefile` issues in lab 3, I've scorched-earth
     replaced the original, overly-fancy approach and will go with a more
     primitive but hopefully bullet-proof one.

  2. Also, the `random` we used in lab 3 was so bad I'm a bit worried about
     what the equivalence checking actually means.  I've replaced it with
     a better version from `libc`.

You should look through `libpi-fake` code, in particular look at how it
handles time, reboot, and the `gpio_*` code.

---------------------------------------------------------------------------------
### Using the linker to pick between different emulation strategies

One issue is what fake things at:

   1. We could simply replace "high-level" `libpi` routines with a
      simple print.  For example `libpi-fake/fake-gpio.c` does exactly
      this for the `gpio_*` functions, which simply print out what pin
      they were called on.  This approach is easy to build and naturally
      makes it easy for a client to see what's going on.  However, since
      they skip everything about the implementation, they obviously
      can't find errors in it --- such as our own version of `gpio.c`
      as in the last lab.

   2. We could only provide a low-level emulation (e.g., just provide
      `PUT32` and `GET32`).  This lets us pull in any `libpi` code that
      uses these low-level routines, but can make it unclear what is
      going on in the case of difference.

As is usual with any trade-off, there is no right answer and sometimes
you want one or the other.  Unlike, perhaps, other situations we can
satisfy our fear of commitment by using simple linker trick.

  1. `libpi-fake` provides high-level implementations of the main
     functions pi programs use, such as `printk`, `reboot`, `gpio_*`,
     etc (you can provide others).  It even provides a simple driver that
     will call your `notmain`, so that entire programs can be checked.

  2. However, if you want to check lower-level versions of routines
     in your code, simply link these routines (as `.o` or `.c` files) before
     `libpi-fake.a` in the link command of the program you are checking.
     If everything is structured correctly, the linker will use your
     definitions and not ours.   (Discussed more below.)

     For example: to check `gpio.c`, just look at how `fake-put-get.current`
     compiles the testing harness for `gpio.c`:

        gcc -Wall -DRPI_UNIX <...deleted flags...>              \
                simple-gpio-test.c                              \
                /home/engler/class/cs140e-20win/libpi/gpio.c    \
                -o simple-gpio-test                             \
                /home/engler/class/cs140e-20win/libpi-fake/libpi-test.a

    here, because `gpio.c` comes before `libpi-test.a` its `gpio_*`
    routines in `gpio.c` will get linked in, overriding those in
    `libpi-test.a` and tested.

  3. The caveat to our linking hack is that if you override one
     function in an family of related functions (those defined in
     the same `lipi-fake` file) you need to override all of them.
     The reason for this is that (over-simplifying) Unix linkers will
     link "all-or-nothing": if they have to pull in one function from
     a `.o` they will pull in all of them.  If you do not define all
     related routines, the linker will grab the missing ones from the
     higher-level definition, along with the ones you already defined,
     which will produce "symbol multiply defined" errors.

  4. With all this said, I'm a bit nervous relying on this behavior across
     so many different MacOS/Linux/Windows student systems: if we run
     into problems we'll have to go with a different, more manual method
     (perhaps having a low-level `libpi-fake` and a high-level one).
