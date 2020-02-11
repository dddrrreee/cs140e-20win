### Part 0:  switch your bootloader to use the software UART.

Make sure your `replay` code from lab 5 still works.  We will be using
it to check the code here.

### Part 0:  switch your bootloader to use the software UART.

We will be communicating with the ESP8266 over a UART.  If you've done
timings, you know that using your software UART code is not cheap and it
really requires you read promptly.     I initially planned to have you
use the sw-uart to communicate with the ESP8266 --- since that would be
a cool, hard test --- but realistically, if we do that there will not
be a lot of time to do any other computation.  Plus there will be some
tricky errors.

So, instead, we are going to just use the sw-uart code only for
bootloading code and later printing debugging output.  Isolating the
use of sw-uart input to a one-time startup should be much better for
efficiency.  Also, since the bootloader is organized as a send-reply
protocol, we know when input will arrive (and it will only do so when
we ask for it), which makes it both easier to get timing right and more
obvious when things go wrong.

Our first step is to re-structure the bootloader so that it can work
with any set of put-get routines passed into it.  (We will use these
changes later when we bootload code over the network.)

Code changes:
  1. Use the structure provided and initial driver in `0-bootloader-dev`.
  2. As a bonus, you could minimize later changes by detecting if we
     are using the software-uart (send an initial message and see if
     you get a valid reply), and if not fail-over to using hardware.
  3. Make sure you can run the provided binaries.
