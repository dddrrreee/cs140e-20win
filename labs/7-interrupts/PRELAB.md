For this lab, we're going to do interrupts and use them to build a
statistical profiler.  The lab code is going to get rewritten extensively
to make it simpler so will change a bunch --- don't modify it yet.

This prelab is going to get filled out more --- so check it out more later.
You should definitely make
sure to read the two pdf's in the `7-interrupts/docs`:
  - `BCM2835-ARM-timer-int.annot.pdf` --- excerpt from the Broadcom document,
     discusses how to enable both general and timer interrupts.

   - `armv6-interrupts.annot.pdf` ---  excerpt from the ARMv6 document in 
     our main `doc` directory.  Discusses where and how interrupts are delivered.
     You should figure out where the handler lives, and registers are written
     what values, and how to restore them.
