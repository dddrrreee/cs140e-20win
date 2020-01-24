# Prelab for 4-uart: writing your own UART drivers.

What do to before lab:

   0. `make` at this top level should complete with no errors.
   1. Make sure your `fake-put-get` works in the new (improved!) setup. This should
      be fast.
   2. Read the LAB.
   3. Do multiple close passes over the lab readings, both the Broadcom readings 
      and the UART Wikipedia page.

      This will likely be your first device driver: make sure you spend the 
      time on the readings before lab!

------------------------------------------------------------------------------
### DELIVERABLE: migrate to `libpi-fake` 

Last time showed the utility of cross-checking your code against everyone
else automatically.  We'll use that trick multiple times the rest of
the quarter.  To make it easier you now have a `libpi-fake` at the top
of your class repository.

Our general development style will be to write a new piece of
functionality in a private lab directory where it won't mess with
anything else, test or (better) equivalence check it, and then, migrate
it into your main library (e.g., `libpi`) so it can be used by subsequent
programs.

Before lab Wednesday, you should migrate your `fake-put-get.c` code from
last lab (`3-cross-check`) into the new `libpi-fake` directory.

Steps:
   1. Copy your `fake-put-get.c` code into `libpi-fake` at the top level.
   2. Run the tests in `3-cross-check/fake-put-get.current` (which holds the re-factored
      version of the last lab) as described in its `README.md` instructions.  In 
      particular:

   3. Check that you get the same results as you used to by running:

        make clean
        make USE_BAD_RANDOM=1

      Which uses our original, awful `random` implementation.  You should 
      still get the results:

        ./test-put-get | cksum
        2091088470 10304
        ./simple-gpio-test | cksum
        108713007 384
        ./test-gpio  | cksum
        3380538640 2335
        ./test-gpio 3  | cksum
        3203140722 6715
        ./test-gpio 4  | cksum
        409326025 25274

   4. Now, check that you get the same results I did when you use a better 
      random:

        make clean
        make 

      Output should be:

        ./test-put-get | cksum
        2186576372 9174
        ./simple-gpio-test | cksum
        1988872002 384
        ./test-gpio  | cksum
        3441322806 2335
        ./test-gpio 3  | cksum
        2353837218 6715
        ./test-gpio 4  | cksum
        3210579411 24756

   5. To check that we can give a high-level trace of an entire pi program test
      the `blink` program in the same directory:

        make blink

      You should see:

        ./blink
        TRACE:uart_init:uart
        PI:about to start blink!
        TRACE:gpio_set_output:pin=20
        TRACE:gpio_set_output:pin=21
        TRACE:gpio_write:pin=20, val=1
        TRACE:gpio_write:pin=21, val=0
        TRACE:gpio_write:pin=20, val=0
        TRACE:gpio_write:pin=21, val=0
        TRACE:gpio_write:pin=20, val=1
        <...more output omitted...>

      Make sure you know what is going on!

------------------------------------------------------------------------------
### Using two tty-usb devices.

The easiest approach to dealing with multiple tty-usb devices is to just
specify them on the command line (I'd suggest adding commands in your
`Makefile` that do this for you)

For example, on Linux, the first device I plug in will be `/dev/ttyUSB0`
and the second `/dev/ttyUSB1`.  So I would bootload by doing:

        my-install /dev/ttyUSB0 hello.bin

And running `pi-cat` by:

        pi-cat /dev/ttyUSB1

Also, in general, if your code has called reboot, you do not have to pull
the usb in/out to reset the pi.  Just re-run the bootloader.  (A simple
hack is to look at the tty-usb device --- if it is blinking regularly
that is caused by the first message your bootloader keeps sending every
300ms or so: this tells you it's ready for you to send a program).


A more clever solution would be to make variants of your find_ttyusb code:

  - `find_ttyusb_first:   return the first created `/dev/ttyUSB` ---
    have the bootloader call this.

  - `find_ttyusb_last`: return the last created `/dev/ttyUSB --- have
  `pi-cat` call this .

Then, as long as you plug in the USBs in the right order you don't have
to specify anything.   I'd suggest you do this if you get annoyed enough
since you now how the tools :)
