The two drivers  are in:
  - `trace.c`: the tracing driver (write this first).
  - `replay.c`: the replay driver.

The drivers most do argument handling (which is sorta tedious, and also has to 
be identical for everyone so we can grade things.)

Other code:
   - `bad-unix-side/` has programs that if you run as a `my-install` your tracing
    driver should flag as incorrect.

   - `pretty-print`: a gross bit of code (ran out of time, sorry) that will pretty
     print a log into what the messages are.  should help debugging.
