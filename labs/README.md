# The labs

This is only a rough guide ---- we are currently doing significant
lab rewrites.

### Part 1: Going down.

The first chunk of the class will be writing all the low-level code needed
to run the pi and using modern techniques to validate it.  Doing so will
remove all magic from what is going on since all of the interesting code
on both the pi and unix side will be written by you:

  - [0-blink](0-blink/): get everyone up to speed and all
      necessary software installed.    This should be a fast lab.

  - [1-gpio](1-gpio/): write your own code to control the r/pi `GPIO`
     pins using the Broadcom document GPIO description.  You will use
     this to implement your own blink and touch sensor.

  - [2-bootloader](2-bootloader/): you will implement your own
     bootloader to transfer the code from your laptop to the pi.  The
     most common r/pi bootloader out there uses the `xmodem` protocol.
     This approach is overly complicated.  You will do a much simpler,
     more likely to be correct protocol.  It has the advantage that it
     will later make it easier to do a network boot.

  - [3-cross-check](3-cross-check/): you will use read-write logging
     to verify that your GPIO code is equivalent to everyone else's.   If one
     person got the code right, everyone will have it right.

     A key part of this class is having you write all the low-level,
     fundamental code your OS will need.  The good thing about this
     approach is that there is no magic.  A bad thing is that a single
     mistake makes more a miserable quarter.  Thus, we show you modern
     tricks for ensuring your code is correct.

  - [4-uart](4-uart/): you write your first real device driver,
     for the UART, using only the Broadcom document.  At this point,
     all key code on the pi is written by you.  You will use the cross
     checking code from lab3 to verify your implementation matches
     everyone else's.

     A great way to understand the UART hardware is to write your own
     software version of it and use this to communicate with your laptop.
     Doing so gives you a second source of output, which will turn out
     to be very useful later when we do networking.


  - [5-replay](5-replay/): in a twist on lab3, you will use Unix
     system calls to interpose between your Unix and pi bootloader code,
     record all reads and writes, and test your bootloader implementation
     by replaying these back, both as seen and with systematic
     corruption.

     This approach comes from the model-checking community, and I believe
     after you implement this lab and test (and fix) your bootloader you
     will be surprised if it breaks later.  (In general, the approach
     we follow here applies well to other network protocols which have
     multi-step protocols and many potential failure modes, difficult
     to test in practice.)

At this point you have written low-level device driver code, a bootloader,
and pretty much replaced all of our code.  You've learned how to crush
it under a barrage of automatic checks based on read-write equivalance
and simple systematic exploration  so that you will be surprised if the
code ever breaks.

We are now going to switch gears to intensively implementing core OS
functionality: threads, interrupts, virtual memory and file systems.

### Part 2: Threads and Interrupts, with Tricks:

  - [6-threads](6-threads/): we build a simple, but functional
  threads package.  You will write the code for non-preemptive 
  context switching:  Most people don't understand such things
  so, once again, you'll leave lab knowing something many do not.

***[HAVE NOT UPDATED THE LABS BELOW]***

  - [8-interrupts](8-interrupts/): you will walk through a simple,
  self-contained implementation of pi interrupts (for timer-interrupts),
  kicking each line until you understand what, how, why.  You will
  use these to then implement a version of `gprof` (Unix statistical
  profiler) in about 30 lines.

     Perhaps the thing I love most about this course is that because we
  write all the code ourselves, we aren't constantly fighting some
  large, lumbering OS that can't get out of its own way.  As a result,
  simple ideas only require simple code.  This lab is a great example:
  a simple idea, about twenty minutes of code, an interesting result.
  If we did on Unix could spend weeks or more fighting various corner
  cases and have a result that is much much much slower and, worse,
  in terms of insight.

  - [9-preemptive-threads](9-preemptive-threads): you will use the
  timer-code in the previous lab to implement pre-emptive threads along
  with locks and some simple checking code.

  - [10-ESP](10-esp-int/): in the capstone lab, we implement an
  interrupt-based UART channel and use this to control the ESP 8266
  wireless chip, which you can use to communicate with other pi's.

### Part 3: file systems.

  - [11-fuse-fs](11-fuse-fs/): In this lab you will use the FUSE file
    system to wrap up your pi as a special file system and mount it on your
    laptop, where you can use standard utilities (and your normal shell)
    to interact with it.  You can then control the pi by reading / writing
    to special files: e.g., echoing a `1` to `pi/reboot` to cause a reboot,
    echoing a program to `/pi/run` to run it.

    This lab is a great example of the power of Unix's simple, powerful
    OO-interface that lets you package a variety of disparete things as
    files, directories, links and interact with them using a uniform set
    of verbs (e.g., `open()-read()-write()-close()`).

  - [12-simple-fs](12-simple-fs): here you will write code to partition
  your pi's SD card driver and write a custom file system to manage it so that
  you can read/write files across runs.

### Part 4: Virtual Memory

  - [14-vm.0](14-vm.0/): Virtual memory and the SD card file
  system are the biggest unknowns in our universe, so we'll bang out
  quick versions of each, and then circle back around and make your
  system more real.  This lab is a semi-lecture on the big picture of
  virtual memory, then you take a working VM system and replace its page
  table manipulation with your own.

  - [15-vm.1](15-vm.1/): The previous lab defined the main noun
  in the virtual memory universe (the page table); this lab does the main
  verbs used to set up the VM hardware, including how to synchronize
  hardware, translation, and page table state (more subtle than it
  sounds).  At the end you should be able to delete all our starter code.

#### Part 5: processes

  - [16-syscall](16-syscall):
    today we're going to build a simple system call.  Once we have this
    in place, we can then make user-level processes --- without it,
    they would not even be able to call `exit()`.

    As a secondary purpose, this lab will get you more comfortable
    with interrupts since it will require you do invasive surgery on the
    timer-interrupt code from `lab7-interrupts` in order to repurpose it
    to support system calls.

  - [17-user-level](17-user-level): today you will make user level
    processes.  This will combine the virtual memory, system calls, and
    threads into a capstone implementation.  You will use this ability to
    your fuse-FS support multiprocessing, so that you can have multiple
    programs running at the same time.

### Optional labs:

  - [virtualization](virtualization/): this lab will show how
  to virtualize hardware.  We will use simple tricks to transparently flip
  how your pi code is compiled so you can run it on Unix, only shipping
  the GPIO reads and writes to a small stub on the pi.  As a result,
  you have full Unix debugging for pi code (address space protection,
  valgrind, etc) while getting complete fidelity in how the pi will behave
  (since we ship the reads and writes to it directly).

  - [sonar-int](sonar-int/): we take a bit of a fun break,
  and bang out a quick device driver for a simple sonar device. You
  will then get a feel for how interrupts can be used to simplify code
  structure (counter-intuitive!)  by adapting the interrupt code from
  the previous lab to make this code better.
