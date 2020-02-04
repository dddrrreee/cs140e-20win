## Lab 8:  device interrupts and networking.

We're going to get networking up, and use interrupts to make it 
robust.  We'll be using the [ESP8266](https://en.wikipedia.org/wiki/ESP8266)
chip, which you can buy for $2 or less on ebay and related sites.


This lab has a bunch of moving parts.  The main pieces we need:

   1. A working software UART implementation.  Hopefully this is a minor
      bit of coding to finish.

   2. The helper routines to do GPIO interrupts.

   3. Reading through the ESP8266 PDF files in `docs/`.  You need to read 
      the examples file to see how to setup the ESP as a tcp server and
      sending/receiving tcp packets.  The other documents have more 
      commands (we have an old version of the chip I bought off ebay,
      so some of these won't work).
----------------------------------------------------------------------------
#### Part 1: A cycle-based software uart

You'll need to finish up your software UART implementation from lab 4.
You'll be using this to talk to the ESP8266.   You'll make two changes:
 1. Since the ESP8266 is a cut rate device, I'm not sure how much we
    can rely on it being rebust in the face of errors.  Thus, you
    should switch to making your sw-uart cycle based.

 2. Since the GPIO pins you use for your sw-UART routine do not buffer this
    means that we will lose any input if we do not read "soon enough".  Thus,
    make two routines: 

        // read from uart <u>, storing the results in <buf> until either
        //    1. you read character <end>
        //    2. you run out of buffer space.
        // returns number of characters read (including the last one).
        int sw_uart_gets_until(sw_uart *u, uint8_t *buf, uint32_t nbytes, uint8_t end);

        // read from uart <u>, storing the results in <buf> until:
        //    1. you have not received any input in <timeout> usec.
        //    2. you ran out of buffer space.
        int sw_uart_gets_timeout(sw_uart *u, uint8_t *buf, uint32_t nbytes, uint32_t timeout);

    Between these routines, we can do simple input processing of the ESP without
    using interrupts (as an initial cut).

    In order to minimize the chance we lose characters, include these below the
    implementations of your putc and getc so those routines can be inlined.

***IMPORTANT***:
  - If you already have a working micro-second version,
    don't do the modifications to it directly: make a copy and go from
    there so you always have a working system!)

The general implementation strategy:
 1. Read the cycle count at the beginning of your `sw_uart_get` or `sw_uart_put` routine.
 2. Pass this `start` value to any timing routines so that they can delay
    the requested number of cycles from the given start point, rather than 
    blindly delaying for some amount of time, ignoring the actual time the
    delay was initiated at.

    The above approach means that the error you have in your
    implementation can get corrected at each delay point rather than
    repeatedly compounding (which happens if you just blindly delay
    irrespective of when you start delaying).

You can use the following two building blocks:

    // delay <n> cycles assuming we started counting cycles at time
    // <start>.  
    static inline void delay_ncycles(unsigned start, unsigned n) {
        // this handles wrap-around, but doing so adds two instructions,
        // which makes the delay not as tight.
        while((cycle_cnt_read() - start) < n)
            ;
    }

    // write value <v> to GPIO <pin>: return when <ncycles> have passed since
    // time <start>
    //
    // can make a big difference to make an inlined version of GPIO_WRITE_RAW
    // after you do: if you look at the generated assembly, there's some
    // room for tricks.  also, remove the pin check.
    static inline void
    write_cyc_until(unsigned pin, unsigned v, unsigned start, unsigned ncycles) {
        // GPIO_WRITE_RAW(pin,v);
        gpio_write(pin,v);
        delay_ncycles(start,ncycles);
    }

I will hopefully check in some checksums you can verify against late tonight
after I get the rest of the lab working :)


Hint: To make it easier to test your software UART, I would add two routines:
  1. `find_ttyusb_first`: returns the first created tty-USB.
  2. `find_ttyusb_last`: returns the last created tty-USB.
Change your `my-install` to use the first one, and `pi-cat` to use 
the latter.  Then, as long as you plug in your uart devices in the right
order (the main pi UART first, the other used to communicate with your
software UART second) then everything should just work.
----------------------------------------------------------------------------
#### Part 2: Implement helper functions for GPIO interrupts.

If you keep hacking on embedded stuff, the single most common activities
your code will do is (1) setup a hardware device and (2) then configure
device interrupts over GPIO pins (e.g., in response to setting them high,
low, transition, etc).  So today's lab will involve doing the latter:
set the GPIO pins you are doing SW-uart reception over to give you an 
interrupt when they go from low-to-high and also from high-to-low.  You
can use these transitions to detect when bits are being transmitted.

Before lab, implement four functions (prototypes are in `gpio.h` in `libpi`):

  - `gpio_int_rising_edge(pin)`: enabling rising edge detection for `pin`.  Page 97
    discusses it and the address to write to is on page 90.

  - `gpio_int_falling_edge(pin)`: enabling rising edge detection for
    `pin`.  Page 99 discusses it and the address to write to is on page 90.

  - `gpio_event_detected(pin)`: returns 1 if an event was detected (p96).

  - `gpio_event_clear(pin)`: clears the event set on `pin` (p96: write the same
    address as `gpio_event_detected`).

Don't panic.  These are pretty much like all the other GPIO functions
you've built: get an address, write a bit offset.   No big deal.  

However, just as with timer interrupts, enabling is a two-step process: we first
enable the specific GPIO interrupt(s) we care about using the first two routines
above, and then tell the pi that we care about GPIO interrupts generally.  If you 
look on page 113:
  - We want to enable `gpio_int[0]`, which is `49` in terms of general
    interrupts.  (we use `gpio[0]` since we are using a GPIO pin that
    is less than 32).

  - To set this, we bitwise-or a 1 to the `(49-32)` position in
    `Enable_IRQs_2` (which covers interrupts `[32-64)`).


This appears magic, especially since the documents are not clear.  But,
mechanically, it's just a few lines.   You should look at the timer
interrupt code to see how to set the general interrupts.

At this point you'll have interrupts enabled.

The interrupt handler works by checking if an event happened, and then using the 
`pin` value to determine if it was a rising or falling edge:
    
    if(a GPIO event was detected for pin) 
       if <pin> = 0, then
           it was a falling edge (a transition from high to low, i.e., 1 -> 0).  
       else 
           it was a rising edge (a transition from low to high).
      clear the event

As with timer interrupts, you need to clear the event that generated
the interrupt (using `gpio_event_clear`).

I will post checksums for my functions later tonight.

To make your code cleaner, I'd suggest using something like the following to
make it easier to do your `read-modify-write` of enabled pin events:

        static void or32(volatile void *addr, unsigned val) {
            device_barrier();
            put32(addr, get32(addr) | val);
            device_barrier();
        }
        static void OR32(unsigned long addr, unsigned val) {
            or32((volatile void*)addr, val);
        }

More complete discussion of the routines is in `gpio.h` but to make sure you
don't miss it the comments are cut-and-paste below:

    // gpio_int_rising_edge and gpio_int_falling_edge (and any other) should
    // call this routine (you must implement) to setup the right GPIO event.
    // as with setting up functions, you should bitwise-or in the value for the 
    // pin you are setting with the existing pin values.  (otherwise you will
    // lose their configuration).  you also need to enable the right IRQ.   make
    // sure to use device barriers!!
    int is_gpio_int(unsigned gpio_int);
    

    // p97 set to detect rising edge (0->1) on <pin>.
    // as the broadcom doc states, it  detects by sampling based on the clock.
    // it looks for "011" (low, hi, hi) to suppress noise.  i.e., its triggered only
    // *after* a 1 reading has been sampled twice, so there will be delay.
    // if you want lower latency, you should us async rising edge (p99)
    void gpio_int_rising_edge(unsigned pin);
    
    // p98: detect falling edge (1->0).  sampled using the system clock.  
    // similarly to rising edge detection, it suppresses noise by looking for
    // "100" --- i.e., is triggered after two readings of "0" and so the 
    // interrupt is delayed two clock cycles.   if you want  lower latency,
    // you should use async falling edge. (p99)
    void gpio_int_falling_edge(unsigned pin);
    
    // p96: a 1<<pin is set in EVENT_DETECT if <pin> triggered an interrupt.
    // if you configure multiple events to lead to interrupts, you will have to 
    // read the pin to determine which caused it.
    int gpio_event_detected(unsigned pin);
    
    // p96: have to write a 1 to the pin to clear the event.
    void gpio_event_clear(unsigned pin);

