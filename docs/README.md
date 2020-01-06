#### Summary

These are useful primary documents for the class.  I've annotated some
of them, but not-annotated most.

Note, confusingly and annoyingly: on most browsers, clicking on the
links below **will not** correctly load `.pdf` files, but will instead
just throw their raw contents to the screen.  You can add a plugin,
or just load them externally.

The main documents:

   0.  Cheat sheet of the r/pi A+ GPIO pins [gpio.png](https://github.com/dddrrreee/cs140e-20win/blob/master/docs/gpio.png).

   1.  The r/pi uses the Broadcom BCM2835 chip to control  the various
       pins sticking up on its right side.  The function of these
       pins (and the broadcom chip more generally) is decribed in
       [BCM2835-ARM-Peripherals.annot.PDF](https://github.com/dddrrreee/cs140e-20win/blob/master/docs/BCM2835-ARM-Peripherals.annot.PDF).

       As you will find with all hardware documents, the broadcom
       on has numerous errors, so always be sure to look at [the
       errata](https://elinux.org/BCM2835_datasheet_errata).

   2. We are using the r/pi A+, which is a ARM1176JZF-S ARM11 chip.
      Confusingly, the ARM11 is actually an ARMv6 rev of the
      architecture: [arm11](https://en.wikipedia.org/wiki/ARM11).
  
      The general ARMv6 architecture manual (which describes the entire
      family of chips): [armv6.annot.pdf](https://github.com/dddrrreee/cs140e-20win/blob/master/docs/armv6.annot.pdf).

      The specific `ARM1176JZF-S` manual (which describes specific features
      of the chip we are using):  [arm1176.pdf](https://github.com/dddrrreee/cs140e-20win/blob/master/docs/arm1176.pdf).

   2. We will be doing some assembly programming.  The easiest hack to
      see how to do something in assembly is to write some equivalent C
      code, compile it with the arm `gcc` you install, and disassemble
      the output and examine the emitted instructions.   

      For a more general view see the following documents:

      A quick reference:
      [arm-asm-quick-ref.pdf](https://github.com/dddrrreee/cs140e-20win/blob/master/docs/arm-asm-quick-ref.pdf).

      A chattier book describing assembly concepts:
      [ARMBook.pdf](https://github.com/dddrrreee/cs140e-20win/blob/master/docs/ARMBook.pdf).

      A thorough walk through of ARMv6 instructions: [armv6-inst.pdf](https://github.com/dddrrreee/cs140e-20win/blob/master/docs/armv6-inst.pdf).

      An even more thorough walk-through of ARMv6 instructions: [armv6-inst-full.pdf](https://github.com/dddrrreee/cs140e-20win/blob/master/docs/armv6-inst-full.pdf).
