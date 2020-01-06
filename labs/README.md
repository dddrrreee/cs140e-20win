# The labs

This is only a rough guide ---- we are currently doing significant
lab rewrites.

### Going down.

The first chunk of the class will be writing all the low-level code needed
to run the pi and using modern techniques to validate it:

  0.  [0-blink](0-blink/): get everyone up to speed and all
      necessary software installed.    This should be a fast lab.

  1. [1-gpio](1-gpio/): write your own code to control the r/pi `GPIO`
     pins using the Broadcom document GPIO description.  You will use
     this to implement your own blink and touch sensor.

  2. [2-bootloader](2-bootloader/): you will implement your own
     bootloader to transfer the code from your laptop to the pi.  The
     most common r/pi bootloader out there uses the `xmodem` protocol.
     This approach is overly complicated.  You will do a much simpler,
     more likely to be correct protocol.  It has the advantage that it
     will later make it easier to do a network boot.

  3. [3-cross-check](lab3-cross-check/): you will use read-write logging
     to verify that your GPIO code is equivalent to everyone else's.   If one
     person got the code right, everyone will have it right.

     A key part of this class is having you write all the low-level,
     fundamental code your OS will need.  The good thing about this
     approach is that there is no magic.  A bad thing is that a single
     mistake makes more a miserable quarter.  Thus, we show you modern
     tricks for ensuring your code is correct.

  4. [lab4-uart](lab4-uart/): you write your first real device driver,
     for the UART, using only the Broadcom document.  At this point,
     all key code on the pi is written by you.  You will use the cross
     checking code from lab3 to verify your implementation matches
     everyone else's.

  5. [lab5-replay](lab5-replay/): in a twist on lab3, you will use Unix
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

  6. [lab6-virtualization](lab6-virtualization/): this lab will show how
  to virtualize hardware.  We will use simple tricks to transparently flip
  how your pi code is compiled so you can run it on Unix, only shipping
  the GPIO reads and writes to a small stub on the pi.  As a result,
  you have full Unix debugging for pi code (address space protection,
  valgrind, etc) while getting complete fidelity in how the pi will behave
  (since we ship the reads and writes to it directly).

### Threads and Interrupts, with Tricks:

  7. [lab7-interrupts](lab7-interrupts/): you will walk through a simple,
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

  8. [lab8-sonar-int](lab8-sonar-int/): we take a bit of a fun break,
  and bang out a quick device driver for a simple sonar device. You
  will then get a feel for how interrupts can be used to simplify code
  structure (counter-intuitive!)  by adapting the interrupt code from
  the previous lab to make this code better.

  9. [lab9-threads](lab9-threads/): we build a simple, but functional
  threads package.  You will write the code for non-preemptive and
  preemptive context switching.  Most people don't understand such things
  so, once again, you'll leave lab knowing something many do not.

### Packaging up the pi: Shells, file systems, more.

  10. [lab10-shell](lab10-shell/): You'll write a simple shell that runs 
  on Unix and can:
      + Remotely execute a set of built-in commands (`reboot`, `echo`)
      on the pi using a simple server.

      + Ship pi binaries to the pi-server, where they execute, and their
      output echoed on the Unix-side shell's console (as you did in your
      lab5's handoff test).

      + Run Unix commands locally so that, for example, `ls`, `pwd`, work.

      While it sounds like a lot, you've done much of the hard parts,
      and can just re-purpose old code (bootloader, replay, system call
      tricks) --- we're at the point in the quarter where you start to get
      some nice technological velocity because of how much you've done.

  11. [lab11-fuse-fs](lab11-fuse-fs/): while building a shell is
  illuminating, you'd like to have the pi more integrated into your
  computing, versus having to use a special-purpose interface to talk
  to it.  In this lab you will use the FUSE file system to wrap up your
  pi as a special file system and mount it on your laptop, where you can
  use standard utilities (and your normal shell) to interact with it.
  Writes to special files (`/pi/reboot`, `/pi/run`) will take the place
  of built-in shell commands.  

      This lab is a great example of the power of Unix's simple, powerful
  OO-interface that lets you package a variety of disparete things as
  files, directories, links and interact with them using a uniform set
  of verbs (e.g., `open()-read()-write()-close()`).

### Virtual Memory

  12. [lab12-vm.0](lab12-vm.0/): Virtual memory and the SD card file
  system are the biggest unknowns in our universe, so we'll bang out
  quick versions of each, and then circle back around and make your
  system more real.  This lab is a semi-lecture on the big picture of
  virtual memory, then you take a working VM system and replace its page
  table manipulation with your own.

  13. [lab13-vm.1](lab13-vm.1/): The previous lab defined the main noun
  in the virtual memory universe (the page table); this lab does the main
  verbs used to set up the VM hardware, including how to synchronize
  hardware, translation, and page table state (more subtle than it
  sounds).  At the end you should be able to delete all our starter code.


### Project lab

  14. [lab14-watch](lab14-watch/): This is a light lab so you can
      kick-start your final project: brainstorm, write some initial code,
      look through the devices we have to place some orders for some others.
      However, since this is cs140e, we will still write some code and have
      a checkoff.

      You'll build a tool `cmd-watch` that will run any command (given as
      command line arguments) if any source file in the current directory
      changes.  This should help you the rest of your coding career
      since it allows you to script an entire sequence of operations
      that happen immediately and automatically as soon as you save any
      file you are working on.

  15. [lab15-syscall](lab15-syscall): Today is a relatively light lab
      so you have more time for projects.  We're going to build a
      simple system call.  Once we have this in place, we can then make
      user-level processes --- without it, they would not even be able
      to call `exit()`.

      As a secondary purpose, this lab will get you more comfortable
      with interrupts since it will require you do invasive surgery on the
      timer-interrupt code from `lab7-interrupts` in order to repurpose it
      to support system calls.

### Final labs.

  16. Implement a simple FAT32 file system using an SD card driver so you
  can read/write files to the SD card.

  17. Free-form lab work on final project.

  18. Final project presentations.
