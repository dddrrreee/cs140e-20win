We modify `my-install` to make it easy for different programs communicate
with the pi by having `my-install` hand off the file descriptor it
has for the `tty-USB` device to a client process that it forks/execs.
For example,

        my-install hello.bin -exec pi-echo

tells `my-install` to bootload `hello.bin` (as before) and then fork/exec
`pi-echo` passing the open file descriptor to the `/dev/ttyusb` device
it opened.  The subprocess `pi-echo` can then use this descriptor to
echo input/output to/from the pi and your laptop (i.e., identically to
the default `my-install` behavior).

We do things this way because after we bootload a pi program, we want
to hand-off management of the pi to a variety of different programs,
rather than building in a bunch of different options into `my-install`.
For today's lab, this method lets us bootload and then hand off management
of the pi to a `pi-fuse` file system we build.

More generally, by communicating only through a file descriptor (rather
than having each program find/open the tty), we can easily compose new
behaviors.  For example: we transparently change our `pi<--ttyusb-->unix`
connection instead go over an esp8266 by putting a Unix program (let's
call it `pi-net`) between them `pi<-->pi-net<--->unix` and having it
send and receive messages between your laptop and an esp-enabled pi.
It could also broadcast to many different pi's and run them in lockstep.

In a sense we are doing program-level object-oriented programming: by
having each program communicate with the others over a file descriptor,
we can plug many different ones together, without worrying about how
they actually implement their functionality.  (You could perhaps argue
that 
this is closer to doing templatized generic programming over programs
--- in either case, we can easily compose unanticipated behaviors.

## What to do

You should look over `my-install.c` and `pi-echo.c` to see the changes.
There are a set of simple modifications we made to our original
`my-install` to cut down on the copy-and-paste we've fallen into doing
on some of the later labs:
  1. Changed it to use the main `libunix` (probably should have always done this).
  2. Put `simple-boot.h` in `libunix` so its easy to find.
  3. Refactor out the echo code into `pi_echo` (in libunix).

You should:
  1. Copy your `simple-boot.c` into this this directory and make sure it builds.
  2. Make sure that `my-install hello.bin` and 
     `my-install hello.bin -exec pi-echo` both work.
  3. If you didn't already do so, make sure you have implemented `read_timeout`
     in `libunix`.

