## Lab 8: device interrupts and networking.

***MAKE SURE YOU DO THE [PRELAB](PRELAB.md) FIRST!***

***STILL MODIFYING THIS FILE DRASTICALLY***

Check-off:
  1. Your `gpio` code passes the cksum tests.
  2. You can do loop back `putc/getc` with interrupts
  3. You have a simple ping-pong between you and your partner over the esp8266.

------------------------------------------------------------------------
### Part 1: Implement raw GPIO interrupt handling.

NOTE: first make sure your different GPIO helper routines work.  (See the 
[PRELAB](PRELAB.md).

You're going to detect when the input transitions from 0 to 1 and 1 to 0.
To reduce the change we can a spurious spike from noise we are going to
use the GPIO ... that will wait for a `011` for low-to-high and a `100`
for high-to-low.  (Note that we could do such suppression ourselves if
the pi did not provide it.)

For this test you will just write to the GPIO pins directly and 
check that you received the  expected event.
   1. Set GPIO pin 20 to high.
   2. Enable both the low-to-high and high-to-low interrupts.
   3. Record that you are going to set the pin low, 
      set the pin low (`gpio_write(20, 0)` and check that the 
      interrupt handler triggered and could tell.
  4. Record that you are going to set high, and set high.  Make 
     sure you receive it.
  5. Spin in this loop over and over and make sure nothing gets
     lost.

------------------------------------------------------------------------
### Part 2: implement a interrupt-based `sw_uart_getc`.

Now that you have a working interrupt implementation, you're going to
use it to implement your software uart more reliabily.

Recall that our input line is by default at `1`.  Thus,
a when a character `p` (`0111 0000`) arriving on a pin will look like:
   1. `1->0`: start bit:  Transition 1.
   2. `0->0`: bit 7 of `p` (0):  No transition.
   3. `0->1`: bit 6 of `p` (1): Transition 2.
   4. `1->1`: bit 5 of `p` (1): No transition.
   5. `1->1`: bit 4 of `p` (1): No transition.
   6. `1->0`: bit 3 of `p` (0): Transition 3.
   7. `0->0`: bit 2 of `p` (0): No transition.
   8. `0->0`: bit 1 of `p` (0): No transition.
   9. `0->0`: bit 0 of `p` (0): No transition.
   10. `0->0`: stop bit (0): No transition.
   11. `0->1`: end of transmission, line is high again.

If we record these, we will have the timestamps of three transitions, and
the direction (it can be reconstructed, but we just keep things simple).
We can check that the total covers a time period of `(10 * cycles /
bit)` and reject if not.  Otherwise, we can then use the cycles per bit
to sample in these values, pulling out the reading at: t/2, t + 1/2,
2t + 1/2, etc

You should be able to test this code by seting up a "loop-back" where
your tx connects to your own rx.  You can then:
  - call `uart_sw_putc`, which will trigger transitions picked up by then
    handler
  - call `uart_sw_getc_int` to reconstruct the character.
