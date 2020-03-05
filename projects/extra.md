 - A tuned logic analyzer (or with an ADC, an oscilliscope).  This will
   record the cycle when a pin (or set of pins) transitions from low to
   high or high to low.  you can use this to analyze how accurately a
   piece of hardware / software is doing a hardware protocol.  E.g., you 
   could measure the software UART code to see how many cycles it was off.
   Or if you have real-time thread, just hit a pin when each gets scheduled
   and use this to test how accurate the scheduling is.  Or, if you have some
   working code from an external source, you can monitor the bus to 
   reverse engineer how it works.

   I believe we can get in the 10s of nanoseconds with careful tuning.
   According to Mark Horowitz this is better than logic analyzers that
   cost hundreds of dollars.  I'd be interested in how much you can beat
   them by.

   A logic-analyzer may not sound like a very OS-class topic, but it turns
   out its a great way to thoroughly understand how the machine and the
   generated code (from gcc) works.   Speeding up code is often a way to
   really understand a system since time cuts through all interfaces.
   From trying to tune an analyzer, it seems that getting accuracy
   (with few spikes) is an even better way.

   This will involve looking carefully at gcc-generated assembly or
   writing your own both to generate pulses at known times and to
   record them.

 - Tests!  This might sound like a low-IQ kind of project, but thinking
   carefully about the assumptions of various pieces of code (e.g.,
   vm) and detecting how they are broken requires both understanding
   and cleverness and is a great way to get a deep understanding how
   things work.

   This project would involve writing an aggressive set of tests that
   could blow up as much of our code as possible.  You could also develop
   a more godzilla set of tracing methods to get a more incisive view
   of what code does (rather than just checking its output at the end).

   These would get bundled into next year for sure.

  - A real bare-metal wireless driver.  This is likely a pretty hard
    project, but there's lots of people in the wild that would love it.

    Similarly, taking the SD card driver (or another) and
    aggressively tuning it should make orders of magnitude difference
    in bandwidth/speed.  This is probably easier than wirelesss by
    10x at least.  It's also very useful for other people out there.
    (E.g., search "raspberry pi bare metal sd driver").

