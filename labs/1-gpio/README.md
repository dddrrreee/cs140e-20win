## Lab: write your own code to control the r/pi; throw ours out.

*Important*: as always, read and complete the [PRELAB](PRELAB.md)
before lab!

The first lab was just setup.  Today we get to the fun part: you'll 
use the Broadcom document (`../../docs/BCM2835-ARM-Peripherals.annot.PDF`)
to figure out how to write the code to turn the GPIO pins on/off yourself
as well as reading them to get values produced by a digital device.
You'll use this code to blink an LED and to detect when a capacitive touch sensor is
touched.

Sign off: to get credit for the lab show the following:

   1. That [part1-blink](https://github.com/dddrrreee/cs140e-20win/tree/master/labs/1-gpio/part1-blink) blinks two LEDs on pin 20 and 21 in opposite orders (i.e., if 20 is on, 21 should be off and vice versa).  (This will point out a subtle mistake people make reading the docs).

   2. That [part2-touch](https://github.com/dddrrreee/cs140e-20win/tree/master/labs/1-gpio/part2-touch)
   turns off the pin 20 LED when the touch sensor is touched.

--------------------------------------------------------------------------
### Part 0.  Background on how to think about the Broadcom document.

The r/pi, like most processors has a bunch of different devices it can
control (e.g., the GPIO pins we've been using, the SD card reader, etc.)
Obviously, to use these devices, the pi must have a way to communicate
with them.

An old-school, obsolete approach for device communication is to have
special assembly instructions that the pi CPU could issue.  This sucks,
since each new device needs its own set of instructions.

Instead, modern systems use the following hack: they 
give each device its own chunk of the physical address space and code can 
read or write these locations with magic, device-specific values 
to communicate with the device.   

For example, to turn on GPIO pin 20, we look in the Broadcom document:
   1. On page 95 states that a write to the ith bit of `GPSET0` will set
      the ith GPIO pin on.

   2. Using the table on page 90 we see `GPSET0` is located at address 
      `0x7E20001C` (note: a constant prefixed with `0x` this given in hex notation.)

   3. Finally, just to confuse things, we know after staring at the diagram on 
      page 5 that the broadcom "CPU bus addresses" at 
      `0x7Exx xxxx` are mapped to physical addresses at `0x20xx xxxx` on the pi.  
      Thus the actual address we write is `0x2020001C`.
      (Note: such ad hoc, "you
      just have to know" factoids are wildly common when dealing with
      hardware, which is why this is a lab class.  Otherwise you can get
      stuck for weeks on some uninteresting fact you simply do not know.
      Hopefully, after this class you operate robustly in the face of such nonsense.)

The result of all this investigation is the following sleazy C code:

        *(volatile unsigned *)0x2020001C = (1 << 20);

I.e., cast the locations `0x2020001C` to a `volatile unsigned` pointer and write  (store) the
constant produced by shifting a 1 to the 20th (`1 << 20`) position there:
   - `volatile` tells the compiler this pointer is magic and don't optimize its use away.
   - `unsigned` on the pi is 32-bits.   

Morally the above is fine, despite what some people might tell you.
However, empirically, it is very easy to forget a `volatile` type
qualifier, which will cause the compiler to (sometimes!) silently remove
reads or writes.  In this class we will *never* directly read or write
device memory, instead we will call the procedures `get32` and `put32`
to read and write addresses.  For example:

        put32(0x2020202, (1 << 20));

`put32` will call out to assembly code (`gcc` currently cannot optimize
this) writes the given value of the second argument to the address
specified by the first.   In addition, by using `put32` and `get32`,
it becomes trivial to record all the reads and writes that are done to
device memory, and later use this for testing.  (You will use this in the
next lab to sort-of prove that the code you write in this lab is correct.)

Generally, whenever you need to control a device, you'll do something like
the following: 

  0. Get the datasheet for the device (i.e., the oft poorly-written PDF that describes it).
  1. Figure out which memory locations control which device actions.
  2. Figure out what magic values have to be written to cause the device to do something.
  3. Figure out when you read from them how to interpret the results.

Devices typically require some kind of initialization, a sequence of
writes to kick-start the device down a set of actions, and some number
of reads when we know data is (or could be) available.

In our case, we want to get the GPIO pin initialized to be an output pin 
(to control and LED) or input 
(so that we can read the value being produced by the device).  So:

   1. Figure out what location to write to set a pin to input or output.
   2. Implement `gpio_write` to write the value of an output pin (LED).
   3. Implement `gpio_read` to read the value of an input pin (touch sensor).

While a lot of this may sound complicated, and the Broadcom document
is not particularly friendly, you will see that what is actually going
on is pretty simple, there is just a bunch of jargon, a few loads or
stores to weird addresses, and with a few lines of code you can start
to control some pretty neat stuff.

--------------------------------------------------------------------------
### Part 1.  Make blink work. (30 minutes)

You'll implement the following routines in `gpio.c`:
   1. `gpio_set_output(pin)` which will set `pin` to an output pin.  This should 
       take only a few lines of code.
   2. `gpio_set_on(pin)` which will turn `pin` on.  This should take one line of code.
   3. `gpio_set_off(pin)` which will turn `pin` off.  This should take one line of code.
   4.  After doing so, wire up the LED pins to pin 20 and 21, power-cycle
       the pi, and use the bootloader to load the code:

             % pi-install blink.bin

       Don't forget to make `blink.c` into binary!

Hints:
   1. You write `GPFSELn` register (pages 91 and 92) to set up a pin as an
      output or input. You'll have to set GPIO 20 in GPFSEL2 to output.

   2. You'll turn it off and on by writing to the `GPSET0` and `GPCLR0`
      registers on page 95.  We write to `GPSET0` to set a pin (turn it on)
      and write to `GPCLR0` to clear a pin (turn it off).

   3. The different `GPFSELn` registers handle group of 10, so you 
	  can divide the pin number to compute the right `GPFSEL` register.

   4. Be very careful to read the descriptions in the broadcom document to
      see when you are supposed to preserve old values or ignore them.
      If you don't ignore them when you should, you can write back
      indeterminate values, causing weird behavior.  If you overwrite old
      values when you should not, the code in this assignment may work,
      but later when you use other pins, your code will reset them.
 
               // assume: we want to set the bits 7,8 in <x> to v and
               // leave everything else undisturbed.
               x &=  ~(0b11 << 7);   // clear the bits 7, 8  in x
               x |=   (v << 7);   // or in the new bits
                          
   5. You will be using this code later!   Make sure you test the code by 
	  rewiring your pi to use pins in each group.

--------------------------------------------------------------------------
### Part 2.  Make the touch sensor work.

Part 1 used GPIO for output, you'll extend your code to handle input and
use this to read a capacitive touch sensor.  At this point you have the
tools to control a surprising number of digital devices you can buy on
eBay, adafruit, sparkfun, alibaba, etc.

What you will do below:
   1. Before you write any code: Wire up the touch sensor and make sure the wiring works.
   2. Implement `gpio_set_input(pin)` and `gpio_read(pin)` in `part1-led/gpio.c`.
   3. `cd` into `part2-touch`, compile the code (`make`) and use the bootloader to 
      load it onto your pi and verify it works.
   4. Celebrate.

---------------------------------
##### A. Get the hardware working. (10 minutes)

We always try to test our hardware setup without software since doing so
minimizes the places we need to debug if things do not work. A standard
novice mistake is to wire up everything, write all the code, and ---
when it invariably does not work --  then get stuck on all the different
combinations of things that could be causing problems.  Our approach
of trying to do everything one-at-a-time, in the smallest step we can
think of, dramatically reduces the IQ needed to figure out mistakes.

Note, we are wiring up a bunch of stuff, so it'd be standard to use a breadboard
(for a [tutorial](https://learn.sparkfun.com/tutorials/how-to-use-a-breadboard)).
However to minimize confusion we will just use female-to-female jumper wires
to connect everything.

In our case, when the touch sensor is touched, it will produce voltage on its
output pin.  We can check that it does so by just hardwiring this pin to an 
LED.
   - Connect the `Vcc` output to the 3v pin on the pi.
   - Connect `Gnd` to one of the ground pins.  
   - Connect `Vout` to the leg of an LED and the LED ground leg to ground.
   - Verify that when you touch the sensor the LED lights up and when you 
     release it, it turns off.

****Put a photo here****.


---------------------------------
##### B. Write the code (10 minutes)

This should be fast:
  1. Implement `gpio_set_input` --- it should just be a few lines of code, which will
     look very similar to `gpio_set_output`.
  2. Hook the `Vout` pin to pin 16 on the pi and an LED to pin 20.
  3. Run the code and see that the LED turns on when you press the button and off
     when you release.
  4. Done!

--------------------------------------------------------------------------
#### 3. Extra: Break and tweak stuff.

You're going to break and change your code to see effects of things going 
wrong and to make it somewhat better:

   1. Change the delay in the blink code to increasingly smaller amounts.   What is going on?

   2. Add the reboot code below (we'll go into what different things mean)
   so that you don't have to unplug, plug your rpi each time:

           // define: dummy to immediately return and PUT32 as above.
           void reboot(void) {
                const int PM_RSTC = 0x2010001c;
                const int PM_WDOG = 0x20100024;
                const int PM_PASSWORD = 0x5a000000;
                const int PM_RSTC_WRCFG_FULL_RESET = 0x00000020;
                int i;
                for(i = 0; i < 100000; i++)
                     dummy(i);
                PUT32(PM_WDOG, PM_PASSWORD | 1);
                PUT32(PM_RSTC, PM_PASSWORD | PM_RSTC_WRCFG_FULL_RESET);
                while(1);
           }
   Change your code to just loop for a small fixed number of times and make
   sure reboot() works.

   3. Force the blink loop to be at different code alignments mod 64.   Do 
   you notice any difference in timing?  (You may have to make your 
   delay longer.)  What is going on?  

--------------------------------------------------------------------------
#### Additional information

More links:

  1. Useful baremetal information: (http://www.raspberrypi.org/forums/viewtopic.php?t=16851)

  2. More baremetalpi: (https://github.com/brianwiddas/pi-baremetal)

  3. And even more bare metal pi: (http://www.valvers.com/embedded-linux/raspberry-pi/step01-bare-metal-programming-in-cpt1)

  4. Finally: it's worth running through all of dwelch's examples:
  (https://github.com/dwelch67/raspberrypi).
