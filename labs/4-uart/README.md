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
   2. Your bootloader works just using your `uart.o` and the libpi.
   3. `hello.fake` using your `uart.c` gives the same hash as everyone
      else.  Note, there are multiple ways to do same thing, so maybe
      do the first one as a way to resolve ambiguity.
   4. Your software UART can reliably print and echo text between the pi
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

  * How to receive/send data.  The miniUART has a fixed sized transmit
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

You will write the code `uart_init()` to initialize the mini-UART
to the state the TTY-USB needs and write `uart_getc` and `uart_putc`
routines to receive and transmit bytes using it, respectively.

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

Wiring up the software UART is fairly simple: 

  1. You'll need two GPIO pins, one for transmit, one for receive.  
     Configure these as GPIO output and inputs respectively.
  2. Connect these pin's to the CP2102 tty-usb device (as with the 
     hardware remember that `TX` connects to `RX` and vice versa).
  3. Connect the tty-usb to your pi's 5v power and ground.
  4. When you plug it in, the tty-usb should have a light on it and nothing
     should get hot!

For a given baud rate, you'll need to compute how many micro-seconds
`T` you write each bit.  For example, for 115,200, this is:
`(1000*1000)/115200 = 8.68`.  Given that we will have various overheads
we round this down to `T=8` seconds.

As described in 
[UART](https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter)
the protocol to transmit a byte `B` is pretty simple:
  1. write a 0 (start) for T usec.
  2. write each bit value in the given byte for T usec (starting at bit 0, bit 1, ...).
  3. write a 1 (stop) for T.

Adding input is good, and not too different.

The code is in `4-uart/sw-uart`:

  1. Your software UART code goes in `sw-uart.c`.
  2. You can test it by running `sw-uart/hello`.

Note, testing is a bit more complicated:
  1. When you connect your pi and figure out its `/dev` name; you will
     give this to the `pi-install` code.

  2. Now connect the auxiliary UART, and figure out its `/dev` name.
     You will give this device name to `pi-cat` which will echo everything
     your code emits.

### Extension: speed.

It's a fun hack to see how high of a baud rate you can squeeze out of 
this code. Note: that if you switch baud rates on the pi, you'll 
need to change it in the `pi-cat` code too.   In the
end I wound up switching from micro-seconds to using the pi's cycle
counters (`cycle-count.h` has the macros to access) and inlining
the GPIO routines.  (Note that as you go faster your laptop's OS
might become the problem.)

