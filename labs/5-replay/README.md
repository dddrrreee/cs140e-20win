## Unix tricks to control processes

***NOTE: this is still going heavy modification.  The basic idea will be the 
same in the lab but some details my change.***

This lab will show how to use Unix verbs (`fork`, `exec`, `waitpid`)
and nouns (sockets, pipes, processes) to control processes and make them
do tricks.

Our main trick will be testing your bootloader code by using another
process to intercept all its `read` and `writes` to check equivalance
and to it to fail in a variety of ways, making sure it will not break
in weird situations.

### Background

The Unix and pi side of your bootloader can be viewed as two nodes in
a networked system (two independent processes that run and send and
receive messages over the wires connecting the pi and your laptop).
Testing such code is hard:

  1. Just as we have with the UART and GPIO implementations we would
     like to show equivalance between implementations (especially as you
     take your simple implementation and make it fancier).  However,
     just running the code raw, we have no insight into what is going
     on between the two ends. 

  2. There are generally many different combinations of events that
     can happen (messages could be lost, corrupted, interleaved in
     different orders, etc) but a given execution will only check one
     and you won't have any good way to exhaust all the possibilities.
     For example, in our bootloader we would like to verify that
     corrupting any byte between the unix and pi will be detected, but
     clearly running the code over and over is not an effective method.
     Similarly, to make the pi-side suitable for network booting we
     want to make sure that a timeout on any message read will cause
     the pi-side to reset itself without any manual intervention.

Roughly speaking, testing such a setup requires some way to interpose
between the messages sent on both side, both to passively trace them
(to compare implementations using read-write equivalance) and to actively
inject faults into them (to systematically check recovery).

Today you're going to build one general-purpose way to do both by using
standard Unix operations.

When you finish the lab, you should be well on the way to having a
correct bootloader implementation that can interoperate with everyone
else's, and, as a result, easily get 100% on our homework test cases.
More importantly, these tricks apply in many other places.

NOTE:

   - We have spent a lot of time checking pieces of the bootloader.  While
     doing so is important (you want the low level of your system to be
     rock solid since everything depends on it, and bugs in it will be
     hard to track down) we are mainly using the bootloader as a simple
     "hello world" example to show off techniques you would use to check
     much larger, real world pieces of code in a full distributed system
     or that deals with low level hardware.  By applying these tricks
     to a small piece of code you understand, you can focus more on the
     tricks themselves.  With that said, this is the last bootloader lab:
     next lab is threads, then interrupts, then other fancy stuff.

----------------------------------------------------------------------
### Part 1: externalize writes

We already saw how to trace read and writes and show code equivalance.
We will extend this approach to system calls by treating Unix `read`
and `write` system calls as loads and stores.  For this part of the lab
you will write a simple shim that can slip between the pi and Unix side
and intercept and log the `read`'s and `writes`'s done between them.
You can then do a simple equivalance check as before.

As we saw, if we modify the code, doing this interception is pretty easy:
We can open a socket at a odd file descriptor number and in `my-install`
check if that decriptor is open: if so, use it, otherwise open the TTY.
However, our puzzle today is to do intercepting without any modification
to the program at all.  This is useful in practice.

We interpose between the pi and unix by writing a program that takes
control of the TTY and forwards and records any communication between
the ends.  Interposing between the pi is easy: just open the TTY-USB
device and we see all of its communicaton.  Our main problem is how to
intercept the communication from `my-install`.  The challenge is that,
as you may recall from the code, `my-install` requires a device that
it can configure as 8N1 (see `set_tty_to_8n1`).  The general way to do
such a thing is to create a pseudo-terminal that `my-install` can open
(see in extensions) and do such operations on.  I had big dreams of
using this approach and spent the weekend working on it.  Unfortunately,
I'm nervous about the semantics of pseudo-terminals across the many
different operating systems everyone is using.  So we will instead use
a simple hack:

  1. Your trace program will run `my-install` as a subprocess
     (using `fork-exec`).  Before doing so, it will open a socket pair
     (see the examples)  and then use `dup` to rename the file descriptor
     intended for `my-install` to a known, odd value (e.g., `21`).
     It should invoke `my-install` with a non-existent device name.

  2. You will modify the `my-install` code in `main` to check if this
     descriptor is open before running:

            int fd = set_tty_to_8n1(open_tty(dev_name), B115200, 1);

     If it is, simply assign `fd` that value instead, otherwise do
     the original code.

There is a `trace.c` that contains some starter code.  The rest is in `interpose.c`.

Your trace code should do the following:
  1. Open the real TTY-USB device, identically to how your `my-install` does.
  2. Run `my-install` as a subprocess that will read/write using a socket file
     descriptor.
  3. Write a loop that listens on both file descriptors (one for the `my-install`
     socket and one for the real TTY) and forward any input from one to the other
     emitting the bytes in a log.
  4. If a read or write fails, check to see if the endpoint is dead.
     Unix side: `waitpid` to check `my-install` has exited using `exit(0)`.
     It flags any other exit (from an abort or a crash) as an error.

To make it easy to compare traces, we name them by a hash of their
contents ---- this way its obvious when you generate a new trace and
its easy to compare different traces.  You should compare the resultant
traces to everyone else.

#### Non-determinism

Note: there are legal ways that the code can be non-deterministic.  For
example:
  1. Depending on timing, there can be a different number of `GET_PROG_INFO` 
     messages sent.  They can also interleave differently with the 
     initial `PUT_PROG_INFO`.
  2. There can be none or some garbage at the beginning of the reads 
     from the TTY device on the unix.
  3. There can be zero or more `PRINT_STRING` messages.

One general way to handle the problem of comparing superficially
different but equivalant executions is to write rules so that they can
be rewritten to some canonical form.  E.g., we could simply delete extra
`GET_PROG_INFO` messages, `PRINT_STRING` messages or any initial garbage.
Since the lab is short, we just run the code a few times --- I'd suggest
doing something smarter as an extension!   Because of how we name files,
you can simply look at the number of files you have to see how many
different executions occured.

----------------------------------------------------------------------
### Part 2: test for failure: corruption.

Given a trace `T`, we want to verify that if we corrupt any byte in it,
that the recepient will detect the error.

We will do the simplest version of checking the Unix side:
  1. Replay a trace.
  2. Corrupt the first byte of data sent to Unix.
  3. Verify that the unix side exits an error.  I.e., `exit(1)`, 
     not `exit(0)` (which is a success) nor a crash, which is never ok.
     Emit this trace into `log.error.corrupt.byte=1.txt`.
  4. Restart the Unix side, replay the trace again from the beginning and
     corrupt the second byte of data and check as above.  Emit the trace
     into `log.error.corrupt.byte=1.txt`.
  5. Keep going until there are no more traces.

You should be able to compare to everyone else.

----------------------------------------------------------------------
### Part 3: test for failure: timeout.

---------------------------------------------------------------------------
### Extension 1: use a PTTY

As we saw, if we modify the code, doing read-write interception is
pretty easy.  However, its better if we can find errors in programs
without needing source.  So our puzzle how to do intercepting without
any modification to the program at all.  For `my-install` it expects we
pass it a device name --- one approach would be to create a "named pipe"
(`man mkfifo`) and pass that.  However, this doesn't work because of the
manipulations it does to the "device" to set it up for 8N1 transmission
(see `set_tty_to_8n1`).  What we can do instead is create a pseudo-TTY
(see `open_fake_tty`) and pass that in instead.  During normal operation,
your code (should) just work.  However, ptty's seem to have slightly 
different rules than sockets so you will have to work out some corner
cases.  It's an interesting problem if you like that kind of thing :).

---------------------------------------------------------------------------
### Extension 2: Do something smart with non-determinism.

Can:
  1. Canonicalize.
  2. Use the self-checking program.
  3. Just use the output from the executed code.
  4. Write a parser that checks the trace and accepts or rejects.

---------------------------------------------------------------------------
## Extension 3: Self-checking programs (20 minutes)

While appending random bytes to the end of a program with known output is
better than testing a single program size, there are plenty of bootloader
bugs it misses (e.g., if the loader simply discards all bytes after
a fixed threshold).   We make this test more rigorous by synthesizing
"self checking programs" ---- programs that know what their contents are,
check this, and emit a "success" message.

The approach is simple: 
  1. Write a pi program that looks at pre-determined addresses to get its
     size and CRC and then use these values to check its entire
     contents.  If the check succeeds it prints success, if not, it prints 
     a failure.
  2. Compile it.
  3. Generate random variations of this code as before, with the change that
     we compute CRCs of each variant, and write the size and CRC to the
     program locations step 1 expects.

As a secondary benefit, doing this will get you more comfortable with
assembly code and where stuff is laid out.

How to do it:
 1. Copy the `libpi/cs140e-start.S` to the `self-check` directory as `check-start.S`.
 2. Modify `check-start.S` so that the second and third instructions in the 
    executable are `nops` and the first instruction of the executable jumps over these
    to the original code. (You could look at the bootloader `start.s` to see how to 
    jump).
 3. Modify main so that it loads the size (offset 4 of the executable) and 
    CRC32 (offset 8 of the executable), computes the CRC of the program from 
    the 12th byte onwards and compare the result to the given CRC.  It should
    print "SUCCESS" or "FAILURE" depending on how this goes.

Debugging:
  1. When you compile check that the `start` code in `check.list` file 
     makes sense (i.e., has a jump, has two nop instructions).
  2. As a first step, simply print "hello world" and make sure this works.


To generate programs:
  1. Compute the CRC over the 12th byte to the end of the code.
  2. Store the size at offset 4 and CRC at offset 8.    Save the file.
  3. We can then run each of these programs to verify that the pi works on them.

This generates many tests for you.

##### Discussion

This check does not just check the boot logic --- it checks how we use
the environment, the compiler, the OS, everything must match up for the
test to succeed.  This approach would have found a very serious bug
that showed up in lab last year that took about 30 minutes to figure
out at the start of an intense lab, and blocked about 1/3 of the class.
Recall from the bootloader code:

   1. The unix side will timeout after T (so that it does not get stuck if 
      the pi dies or locks up).
   2. The pi side computes a CRC of the executable.   Once the shipped
      program gets "large" the CRC can take a significant amount of time
      to compute.   Call this size `S`.
   3. By that part of the quarter people had written a fair amount
      of code.  Most had program sizes `< S`, but about a third had sizes
      `> S`.  For these people suddenly their bootloader stopped working.
      Since we had changed so much (and for whatever reason they were
      all on macs), I assumed it was some weird mac issue and we were
      all wasting a lot of class time looking in the wrong places ---
      it took a while to figure out that the problem was a timeout on
      the Unix side.

Note: while this approach is an improvement, but is not a ***secure***
guarantee --- since the bootloader controls everything, it can skip
running the code entirely and simply print the expected message itself.
However, since we wrote all of our code at this point, we expendiently
assume a lack of malice and stop chasing this type of error.  We have
certainly reached the point that you are surprised if there is an bug in
this part of the code.  
