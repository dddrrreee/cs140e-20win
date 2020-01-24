---
layout: page
title: Quick overview.
show_on_index: true
---

## Lab: write your own UART implementation.

***MAKE SURE YOU DO THE [PRELAB](PRELAB.md) FIRST!***

By the end of this lab, you'll have written your own device driver for
the pi's mini-UART hardware, which is what communicates with the TTY-USB
device you plug into your laptop.  This driver is the last "major" piece
of the pi runtime that you have not built yourself (`cs140e-objs/uart.o`).
At this point, you can print out the entire system, look at each line of
code, and should hopefully know why it is there and what it is doing.
Importantly you have reached that magic point of understanding where:
there is nothing else.

In addition, you'll implement your own *software* UART implementation
that can entirely bypass the pi hardware and talk directly to the
tty-USB device itself.  I.e., without using the driver above, but instead
building an 8n1 serial protocol with the CP2102 device over two GPIO pins.
Multiple reasons:

   1. The most practical reason: this gives you a "console"
      `printk` you can use when the UART is occupied, which turns out to
      be surprisingly useful.  For example, when you write the UART driver
      above, you can't actually use `printk` to debug it, since doing so
      requires a UART!  As another, when we use the ESP8266 networking
      device in a couple of labs: it needs a UART to communicate with
      the pi.  Without a second UART, we would have no way to print to
      the laptop.  Again: hard to debug if you have no idea what the pi
      is doing.

   2. Gives you an existence proof that you don't need to need to use
      hardware but can roll your own.  This applies to all the different
      hardware protocols  built-in to the pi, such as I2C and SPI.

   3. Writing your own version of a hardware protocol gives you more
      of an intuition for the problems that happen at this level..
      For example, how do you deal with concurrency between two entirely
      separate devices when we don't have the usual fall-back on locks,
      etc?  (Hint: the use of messages and very precise time deadlines
      are fairly common.)

   4. This gives you a small demonstration of how your pi setup,
      which appears trivial, is a hard-real time system, where with
      some care, it possible to hit nanosecond deadlines reliably.
      Difficult to do this on a "real" system such as MacOS or Linux.

### Deliverables

Show that:
   1. `uart/hello` works using your `uart`.
   2. Your bootloader works just using your `uart.o` and the libpi (see
      `bootloader/`).
   3.  `hello.fake` using your `uart.c` gives the same hash as everyone
      else.  Note, there are multiple ways to do same thing, so maybe
      do the first one as a way to resolve ambiguity.

   4. Check that after you put your `uart.c` in `libpi` that your
      tracing lab from last time gives the same hash as everyone.
      Note: you should be able just compie and run!  To get the cksum:
      `pi-install hello.bin |& grep TRACE | cksum`.

   5. Your software UART can reliably print and echo text between the pi
      and your laptop.

For your homework, your `uart.c` code should make it clear why you did
what you did, and supporting reasons --- i.e., have page numbers and
partial quotes for each thing you did.

-------------------------------------------------------------------------
#### What to read.

Readings:
  1. Sections 1, 2, 2.1, 2.2, 6.2 (p 102) in the Broadcom document:
     `docs/BCM2835-ARM-Peripherals.annot.PDF` to figure out where, what,
     and why you have to read/write values.
     
     The main reading is pages 8---19.  I had to read the sections about
     10 times (not kidding).   It's do-able.  Lab will ensure we can
     get through it to a working program.

     Note that you need to setup the TX and RX GPIO pins; 6.2 says how.
  2. [errata](https://elinux.org/BCM2835_datasheet_errata) has the
     errata associated with the Broadcom.  
  3. [8n1](https://en.wikipedia.org/wiki/8-N-1)
  4. [UART](https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter).

The main goal of this lab is to try to take the confusing prose and
extract the flow chart for how to enable the miniUART with the baud rate
set to 115200.  You're looking for sentences/rules:

  * What values you write to disable something you don't need
	(e.g., interrupts).

  * What values you have to write to enable things you need (miniUART).

  * How to get the gpio TX, RX pins to do what you need.

  * How to receive/send data.  The mini-UART has a fixed sized transmit
	buffer, so you'll need to figure out if there is room to transmit.
	Also, you'll have to detect when data is not there (for `uart_getc`).

  * In general, anything stating you have to do X before Y.

You don't have enough information to understand all the document.  This is
the normal state of affairs.  So you're going to start practicing how
to find what you need and interpolate the rest.

The main thing is to not get too worked up by not understanding something,
and slide forward to what you do get, and cut down the residue until
you have what you need.

##### Helpful notes:

  1. As with the GPIO, the upper bits of the Broadcom addresses are
  incorrect for the pi and you will have to use different ones (for
  the mini-UART: `0x2021`).

  2.  We don't want: RTS, CTS, auto-flow control, or interrupts, so ignore
  these.

  3. You'll want to explicitly disable the mini-UART at the beginning to
  ensure it works if `uart_init(void)` is called multiple times (as can
  happen when we use a bootloader, or `reboot()`).

  4. Similarly, you will often want to fully enable the mini-UART's ability
  to transmit and receive as the very last step after configuring it.
  Otherwise it will be on in whatever initial state it booted up in,
  possibly interacting with the world before you can specify how it
  should do so.

  5. Since the device can read or write memory invisibly to the
  compiler, we'll manipulate it using `GET32` and `PUT32` (as we 
  did with GPIO) rather than raw pointer manipulations.

  6. You'll notice the mini-UART "register" memory addresses are contiguous
  (table on page 8, section 2.1).  Rather than attempt to type in
  each register's address correctly, just make a large structure of
  `unsigned` fields (which are 32-bits on the pi), one for each entry
  in the table, and cast the mini-UART address to a pointer to this
  struct type.  Since you will be using `GET32` and `PUT32` (as before),
  you don't need `volatile`.

  7.  If a register holds received / transmit data you will almost
  certainly want to clear it before enabling the device.  Otherwise it
  may hold whatever garbage was in there at boot-up.

-----------------------------------------------------------------------
### Part 1. implement a UART device deriver:

Our general development style will be to write a new piece of
functionality in a private lab directory where it won't mess with anything
else, test or (better) equivalence check it, and then, migrate it into
your main `libpi` library so it can be used by subsequent programs.

Concretely, you will implement three routines (skeletons given in
`4-lab/uart/uart.c`):

  1. `void uart_init(void)`: called to setup the miniUART.  It should
     set the baud rate to `115,200` and leave the miniUART in its default
     `8n1` configuration.  Before starting, it should explicitly disable
     the UART in case it was already running (since we bootloader,
     it will be).

  2. `int uart_getc(void)`: blocks until it can read a byte from
     the mini-UART, and returns the byte as a signed `int` (for sort-of
     consistency with `getc`).

  3. `void uart_putc(unsigned c)`: puts the byte `c` onto the UART transmit
     queue.  If necessary, it blocks until there is space.

If you run `make` in `4-uart/uart` it will build:
  - A simple `hello` you can use to test. I'd suggest shipping it over with your
    bootloader.  

  - A `hello.fake` you can run on your laptop to compare against other people.

A possibly-nasty issue: 

  1. If you test using the bootloader (recommended!), that code obviously
    already initializes the UART.  As a result, your code might appear
    to work when it does not.

  2. To get around this issue, when everything seems to work, you can 
     modify `hello` to print in an infinite loop and put it on your SD
     card as `kernel.img` and see if it can successfully print.  You can
     detect if it does by using the `4-uart/pi-cat` program to echo all
     values.

  3. The final test, is to recompile your `bootloader` to use your uart
     and see that it boots the `hello` you used above.

-----------------------------------------------------------------------
### Part 2. implement a software UART.

This part of the lab is from a cute hack Jonathan Kula did in last year's
class as part of his final project.  

While the hardware folks in the class likely won't make this
assumption-mistake it's easy as software people (e.g., me) to assume
things such as: "well, the tty-usb device wants to talk to a 8n1-UART
so I need to configure the pi hardware UART to do so."  This belief,
of course, is completely false --- the 8n1 "protocol" is just a fancy
word for setting pins high or low for some amount of time for output,
and reading them similarly for input.  So, of course, we can do this
just using GPIO.  (You can do the same for other protocols as well,
such as I2C, that are built-in to the pi's hardware.)


Wiring up the software UART is fairly simple: 

  1. You'll need two GPIO pins, one for transmit, one for receive.
     Configure these as GPIO output and inputs respectively.
  2. Connect these pin's to the CP2102 tty-usb device (as with the 
     hardware remember that `TX` connects to `RX` and vice versa).
  3. Connect the tty-usb to your pi's ground.  ***DO NOT CONNECT TO ITS POWER!!***
  4. When you plug it in, the tty-usb should have a light on it and nothing
     should get hot!

As described in 
[UART](https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter)
the protocol to transmit a byte `B` at a baud rate B is pretty simple.

  1. For a given baud rate, compute how many micro-seconds
     `T` you write each bit.  For example, for 115,200, this is:
     `(1000*1000)/115200 = 8.68`.  (NOTE: we will use cycles rather
     than micro-seconds since that is much easier to make accurate.
     The A+ runs at `700MHz` so that is 700 * 1000 * 1000 cycles per
     second or about `6076` cycles per bit.)

To transmit:
  1. write a 0 (start) for T.
  2. write each bit value in the given byte for T (starting at bit 0, bit 1, ...).
  3. write a 1 (stop) for at-least T.

Adding input is good.  Two issues:
  1. The GPIO pins (obvious) have no buffering, so if you are reading from the RX
     pin when the input arrives it will disappear.
  2. To minimize problems with the edges of the transitions being off
     I'd have your code read until you see a start bit, delay `T/2` and then
     start sampling the data bits so that you are right in the center of 
     the bit transmission.

The code is in `4-uart/sw-uart`:

  1. Your software UART code goes in `sw-uart.c`.
  2. You can test it by running `sw-uart/hello`.

An example of how to use the pi cycle counters is in: `cycle-counter-ex`.
Note:
  1. You have to initialize the cycle counters first!
  2. Because the counter will overflow frequently you must be careful
  how you compare values.

Note, testing is a bit more complicated since you'll have two `UART` devices.

  1. When you connect your pi and figure out its `/dev` name; you will
     give this to the `pi-install` code.

  2. Now connect the auxiliary UART, and figure out its `/dev` name.
     You will give this device name to `pi-cat` which will echo everything
     your code emits.
     
     For example, on Linux, the first device I plug in will be `/dev/ttyUSB0`
     and the second `/dev/ttyUSB1`.  So I would bootload by doing:

            my-install /dev/ttyUSB0 hello.bin

     And running `pi-cat` by:

            pi-cat /dev/ttyUSB1

  3. In general, if your code has called reboot, you do not have to pull
     the usb in/out to reset the pi.  Just re-run the bootloader.  (A simple
     hack is to look at the tty-usb device --- if it is blinking regularly
     you know the pi is sending the first bootloader message :)).
  
Note:  our big issue is with error.  At slow rates, this is probably ok.   However,
as the overhead of reading time, writing pins, checking for deadlines gets larger
as compared to `T` you can introduce enough noise so that you get corrupted data.
Possible options:

  - Compute how long to wait for each bit in a way that does not lead to cumulative
      error (do something smarter than waiting for `T`, `2*T`, etc.)

  - The overhead of time is probably the main issue (measure!), so
      you could inline this.  In general, reading time obviously adds
      unseen overhead, so you have to reason about this.

  - You could also switch to cycles.  (our pi runs at `700MHz` or 7 million
      cycles per second.  You should verify 70 cycles is about 1 micro-second!)

  - Unroll any loop and tune the code.

  - Maybe enable caching.

  - Maybe inlining GPIO operations (probably does not matter as long overhead as `< T`)

  - In general: Measure the overhead of reading time, writing a bit,
      etc.  You want to go after the stuff that happens "later" in the bit
      transmit since it has the most issue and the stuff that has a large
      fixed cost (since that will cause the error to increase the most).

  - Could just hand compute an assembly routine that runs for exactly
      the cycles needed (count the jump and return overhead!).  Or,
      better, write a program to generate this code.

  - Part of "systems" as a discipline is actually measuring what effect
      your changes make.  People are notoriously stupid at actually
      predicting where time goes and what will lead to improvements.

      Of course, measuring the actual error is tricky, since the
      measurement introduces error.  One approach is to write a trivial
      oscilloscope program on the pi that will monitor a GPIO pin on
      another pi for some time window (e.g., a millisecond), record at
      what exact cycle count the pin changed value, and print the results
      after. You'll need a partner (or a 2nd pi) but this is actually a
      close-to trivial program and gives a very good measurement of error
      (it's useful in general, as well).

### Extension: speed.

Once you have a handle on error, it's a fun hack to see how high of a
baud rate you can squeeze out of this code using the above tricks.
In the end I wound up switching from micro-seconds to using the pi's
cycle counters (`cycle-count.h` has the macros to access) and in-lining
the GPIO routines.  (Note that as you go faster your laptop's OS might
become the problem.)

  ***NOTE: that if you switch baud rates on the pi, you'll need to change
    it in the `pi-cat` code too.***

