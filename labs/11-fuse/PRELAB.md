There's a fair amount of background reading that could be helpful.
Also some amount of simple coding preperation.

So you don't miss it, for coding:

  1. Start with `0-hello` and work
     through the example, you need to figure out why `open`, `readdir`
     `getattr` and `read` act as they do.  (see part 2 below).

  2. Make sure your `my-install` works (see part 3 below).

Absolutely make sure you understand these five FUSE methods:
   1. `getattr` get attributes of the file: its size, permissions, and
   number of hardlinks to it.

   2. `read`: read fro the file at a `offset` of `size` bytes.

   3. `write`: write to the file at a `offset` of `size` bytes.

   4. `readdir`: return the directory entries in the given directory.

   5. `ftruncate`: truncate the file.

Other than `getattr` (which we already defined), these are more-or-less
identical to Unix calls, so you can just man page them to figure stuff
out.  If you run FUSE with debugging output (which we do) you can see
where it is failing and why (usually permission errors).

***DO NOT WAIT TO DO THE ABOVE TIL LAB.  You will be very far behind.***

---------------------------------------------------------------------
### 1. General File system readings.

General readings:
  1. For the UART interrupt homework, the "concurrency bugs" and 
     [related chapters](http://pages.cs.wisc.edu/~remzi/OSTEP/#book-chapters) are a
     reasonable place to start.
  2. [Files and directories](http://pages.cs.wisc.edu/~remzi/OSTEP/file-intro.pdf).
  3. [File system implementation](http://pages.cs.wisc.edu/~remzi/OSTEP/file-implementation.pdf)
  4. [NFS](http://pages.cs.wisc.edu/~remzi/OSTEP/dist-nfs.pdf), which is what FUSE
     is based on.  Also the cs240 NFS paper could be useful.

There are many other readings at the link above.  Or feel free to ask questions!

---------------------------------------------------------------------
### 2. FUSE file system readings.

The [FUSE file
system](https://en.wikipedia.org/wiki/Filesystem_in_Userspace) has a
bunch of moving parts.  You absolutely have to read about it before lab.

A few places:
   1. [A short, simple writeup](https://engineering.facile.it/blog/eng/write-filesystem-fuse)
   2. [An extended writeup](https://www.cs.nmsu.edu/~pfeiffer/fuse-tutorial/html/index.html).
      This has a more worked out FUSE example, with the various things
      you have to do for many of the operations.
   3. The `FUSE` header file has a thorough set of comments.
      It's located at
      `/usr/include/fuse/fuse.h` on my linux
      system, or [online at the `libfuse`
      repo](https://github.com/libfuse/libfuse/blob/master/include/fuse.h)

    4. There are many example `FUSE` clients at the 
    [github repository](https://github.com/libfuse/libfuse/tree/master/example).


I found it useful to:
   1. Clone the FUSE github repo: `git clone git@github.com:libfuse/libfuse.git`.
   2. Look through the examples in `libfuse/example`.
   3. The `passthrough.c` file system is a good place to start. 
   4. If you're going to compile the libfuse examples you need `meson`
      (`pip3 install --user meson`) and `ninja` (which seems to get
      pulled in when you use `meson`).

To make the reading more goal-directed, start with `0-hello` and work
through the example, you need to figure out why `open`, `readdir`
`getattr` and `read` act as they do.


----------------------------------------------------------------------
## Install FUSE

#### MacOS

From the FAQ (https://github.com/osxfuse/osxfuse/wiki/FAQ):

  - 2.1. What is the recommended way to install "FUSE for OS X"?

         The recommended way to install "FUSE for OS X" is to
         download the latest "OSXFUSE-{version}.dmg" available from
         http://osxfuse.github.com/ and double-clicking on "Install
         OSXFUSE {version}".

From Ellie after she installed on the Mac:

      For Fuse on Mac, I downloaded the Mac version of Fuse, OSXFuse,
      from [here](https://osxfuse.github.io/).  It installs like any
      other application on Mac.

      Alternatively, someone could also use
      Homebrew to install it [using these
      instructions](https://github.com/alperakcan/fuse-ext2#alternate-install-method-of-fuse-for-macos).


#### Linux

From the FUSE `github` repo (https://github.com/libfuse/libfuse):

    apt-get install gcc fuse libfuse-dev make cmake



---------------------------------------------------------------------
### 3. modifying and simplifying `my-install` for the lab.

See the [README](0-my-install/README.md).  You'll need to pull over your
simple-boot.c and make it compile.


---------------------------------------------------------------------
### 4. running remote programs

Your UNIX FUSE file system will provide a way to run pi programs by catting them
to a special file:

        cat hello.bin > /pi/run

And then seeing the output by running:

        tail -f /pi/proc1

At a high level you will re-purpose your bootloader code both (the Unix
and pi side) so that it can send programs from within the shell.
There are some issues with running a pi program while we are currently running.

#### Running multiple programs: Some Issues

Conceptually, running a program on the pi through the shell is pretty simple:

 1. Just copy it over (you already have bootloader code to do so).
 2. Jump to it.
 3. Profit.

Unfortunately, there's a bunch of problems we need to solve for this.  
A subset:

  1. We have to run the code somewhere other than where your already
     running pi code is running --- an identical problem to how we could
     not smash the bootloaders code while shipping a remote binary over.

     Until we get virtual memory, we will have to make sure the program is 
     linked elsewhere.  We also have to keep track of where memory is free.

  2. We need to tell the Unix side when the code is done so that any program
     waiting can go do something else.

  3. What if `hello.bin` crashes?

  4. What if `hello.bin` calls `reboot()` or sets its stack pointer to 
     ours?

  5. What if `hello.bin` infinite loops?

  6. How do we run multiple programs?

  7. How can `hello.bin` call the main program?  For example, if it
     creates threads internally and does a `yield()` this will not be
     able to call the main scheduler.  We might or might not like this
     behavior, depending on context.  Similarly, for `kmalloc`, where
     each program will have its own private heap.  (Sometimes nice for
     isolation, sometimes not so nice because of fragmentation.)

There are more-or-less the problems we'd have to solve to make a full-on
multi-user operating system.  Since we want to get you up and running in
a couple of hours, we solve most of these issues either through gross
hacks or simply pretending they don't exit.  The rest of the quarter
will involve (among other things) eliminating these hacks and solving
the associated problems to make the scheme less embarrassing.

#### Some gross hacks and expedient non-solutions:

For today:

  1. We statically link the code at a "known" address so that when we
     ship it over, your bootloader can simply copy it to that address and
     jump to it, without needing to fix up any absolute addresses the code
     references. (Note that relative addresses "just work" since they
     do not depend on the code's location.)

  2. When the program `hello.bin` finishes on the pi, the pi-side sends
     a special message back to the unix-side so that it will know that
     the pi program is finished.

     Without some form of job-control the Unix-side has no insight into
     if the process finished or not or --- at the most concrete level
     possible --- whether to print a `>` character, signalling it can run
     a new command, or to wait to see if there's another character coming
     that it will need to echo.

  3. If `hello.bin` crashes, we are out of luck: it will crash the entire
     pi-side.  We will be doing virtual memory fairly soon, which gives
     us a way to attack this problem.

  4. We manually rewrite the `hello.bin` code to eliminate `reboot()`
     (which we have so far used as an `exit()`), `uart_init()` (which will
     have already occurred), and change `start.s` to not setup a stack
     pointer internally, but instead just assume that it already has one.

  5. inf-loop: you can actually solve this easily (see extension) even
     with non-preemptive threads.

  6. Multiple programs: just use your threads!  You will need some way
     to track the address ranges that are in use, and some way to know
     when a given thread is finished.  Both require less than 10-20
     lines of code.  For today, we skip this, but you are more than 
     welcome to do this as a homework.

  7. Calling routines in the main pi program: We
     ignore this problem for the moment, but will get to it fairly soon.
     The intuition is that its not hard to do so by using dynamic linking
     or using system calls --- as you already implemented in the first
     interrupt lab.  While we skip this for the moment, in order to
     minimize the many moving parts, you are more than welcome to
     add these.

#### What you will do

##### A. What we do to binaries

To summarize the above:

 1. We statically link each binary to a free address.  
    This means it must be above `HIGHEST_USED_ADDR` (in `rpi-asm.h`) which 
    is the highest address we have ever used in the code
    we've written so far.

 2. We only run one program at a time so don't have to keep track of
    what is free, nor which input/output is for/from which program.

 3. We rewrite the code to avoid any ugly problems such as trashing
    the shell's stack, reboots, or re-initializing the UART.

You can see these concrete changes by examining the code in
`11-fuse/hello-fixed` directory.  You should be able to describe
what the changes are compared to a normal `hello` and why.

##### B. What we do to send binaries

At this point, I was hoping to triumphantly say you would ship programs to
the pi by simply using your bootloading code as-is, thereby vindicating my
foresight and the generality of what you've done already.  Unfortunately,
while you *can* use your code with minimal modifications, I think it's
probably much better to strip the code down to something simpler.
The result will be easier to debug and get around some of the race
conditions in the original protocol, which arise when the protocol's
insufficient flow-control hits the UART's 8-byte finite receive
queue size.

While you won't be using the code as-is, the understanding you gained
from writing it the first time should allow you to create a custom
protocol pretty quickly.  Hopefully.

This is a stripped down version (explained more below):

        =======================================================
         unix side                          pi side 
        -------------------------------------------------------
                                         
        put_byte(fd, PI_PUT_CODE)
        // version: stored by linker
        put_uint(fd, code[0]);
        // address: stored by linker.
        put_uint(fd, code[1]);
        put_uint(fd, nbytes);
        put_uint(fd, crc32(code, nbytes));
        <send code>
                        -------------->
                                           <sanity checks>
                                           <copy code to addr>
                                           <check code crc>

                                           <done!>
        =======================================================


More descriptively:

 1. The unix side sends: PI_PUT_CODE, the address the code is linked at,
 its size, and a CRC of the code (`crc32(code, nbytes))`) and the code.  
 The linker
 script is written to that it stores a magic cookie `0x12345678` (so we
  can sanity check) as the first word of
 the binary, and its linked-to address as the second.  

 2. The pi-side checks the address, the size, and copies the code
  (don't wait or you will blow out the uart buffer).  After copying
  it checks the CRC and if that is ok, jumps to `addr`.

We are sleazily running the code on our stack, so after you bootload,
you can simply jump to it.

#### Done!

At this point you will have a very simple shell!  A full-featured one
is a lot more code, but not alot more ideas.

----------------------------------------------------------------------
## Part 4:  Extensions.

 1. Run the program in its separate thread so main program stack is
 not corrupted.  You can then extend your Unix side so you can run
 multiple programs.  Because we are running cooperatively, we might
 actually be able to make good progress without trashing shared state
 (such as the UART).

 2. Add a `rpi_alarm(code, ms)` function (using timer-interrupts) to your
 threading package so that it will `code` when an alarm expires.  You can
 use this to make a "watch-dog timer that kills a too-long running thread.
 You will have to also add `rpi_kill(tid)`.  Alternatively, you could
 add pre-emption to your thread code, and simply have your pi-code run
 in a loop, yielding control until too-much time has passed, upon which
 it kills the running program.

 3. Compile some other programs using fixed addresses
 and change the delay code to `rpi_yield()` rather than busy wait..

See the [README](0-my-install/README.md).  You'll need to pull over your
simple-boot.c and make it compile.
