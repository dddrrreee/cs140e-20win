## Exporting your pi as a file system.

***NOTE: This is from last year's lab.   It assumes we built a shell.  I think 
not doing so is better, so we won't this year.***

Today you're going to take your pi shell-system and use the FUSE file
system to mount it as a normal file system on your laptop.    Each ability
of the pi will be exposed to the user by mapping it to its own file and
potentially overriding the sematics of `read()` and `write()` with custom
actions.  

For example:

 - `/pi/reboot`: if you write anything to this file, it will reboot the 
 pi.

 - `/pi/echo`: if you write anything to this file, it will be echoed
 by the pi.

 - `/pi/console`: contains all output from the pi.  

 - `pi/run`: writing a pi binary to this file will cause the pi to run
 it (and the output appended to `pi/console`).

This approach lets you control your pi using standard Unix utilities
rather than writing a custom shell to export different actions.  E.g.,
running `tail -f /pi/console` will cause all output from the pi to 
be echoed to your terminal.

#### The big picture

In the previous lab, you wrote a trivial shell that would look at what
the user typed and decide if it was:

  0. A built-in command: run on the pi.
  2. A pi program: run on the pi.
  1. Anything else?  Assume its a unix command: run on your laptop.

Building it was a good proof-of-concept for seeing one way to meld
your usual computing environment with your pi system.  On the other
hand, it's clear it's a dead-end, at least without significant work.
Among many reasons: (1) it is, and will remain, much less full-featured
than your regular shell (no auto-completion, no history, no job control,
no environment variables, etc) , (2) we are far from running any Unix
code on the pi.

In some sense, what we'd like is some way to extend your current shell
(`bash`, `sh`, `tcsh`) so that it does all its normal laptop-stuff,
but calls out to the pi for pi-stuff.  This would likely require having
each different shell have some extensibility method and require us to
write a bunch of uninteresting shell-specific code.

We're going to side-step the problem to make it easier and our solution
better.  One surprise in systems is that it can sometimes be easier
to solve a more general problem than the one you are focused on.
This situation is a good example of that.

Rather than building pi-specific knowledge into the shell, we instead
extend the pi as a file system, which your shell can then work with as
it does any other sets of directories, files, links.

Your shell is already good at dealing with file systems, and your laptop
has lots of programs to do so.  Once we wrap the pi up as a set of files,
directories, links, then you can do everything you wrote in your Unix-side
shell and much much more, while cleanly causing any pi-action to occur.

You now have autocompletion, history, job control, etc without having
to write any code.

Most OO programming --- at best --- allows you to wrap different nouns up
with the same verbs, so that a single piece of code that works on those
verbs can work on any specific instance.  If I could pick one single
success story of this approach, and the one that it simultaneously the
most under-appreciated, it would be the Unix file system interface.

If you can map your thing (e.g., hardware device) to you immediately
have it integrate, fairly cleanly, with all the existing code out there.

Of course, are plenty of things that do not fit the Unix model of
`open()-read()-write()-close()`.  A simple way to get thousands of
examples: `grep ioctl` in Linux device directories and look at all the ---
oft-buggy --- code that was stuffed into these `ioctl`-routines because
it did not fit nicely within the Unix file interface.


### Check-off

You need to show that:

  1. Your pi-FS works as a standard file system.  `make part0` wil
  run some simple tests.  You should be able to run the usual
  Unix file programs, have them work as expected, and have all
  output appear in `/pi/console`.  For example:

           % echo "hello world." > /pi/echo
           % cat /pi/echo
           hello world.
           % echo "bye world." > /pi/echo
           % cat /pi/console
           hello world.
           bye world.


   2. Hook up your pi-FS to your pi-sh.  When the user writes values
   to `/pi/echo`, `/pi/reboot`, `/pi/run` the relevant command is
   sent to the pi (via the shell) and all shell-output is written to 
   `/pi/console`.  


      This will involve writing Unix code to fork/exec
   fork/exec a program, overriding its file descriptors for `stdin`
   (0), `stdout` (1), and `stderr` (2) so that it can interact with the
   program identically as a user typing from the console.

Extensions:

   1. Add directories.
   2. Add `/pi/dev/led` so that writes turn the led on/off and that 
   reads give the current state.
   3. Add a `pthread` thread to your FS that will pull output from
   the pi in the background.

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

----------------------------------------------------------------------
## Part 0: use FUSE (15 minutes)

Run the `hello` example in `lab11-fuse-fs/part0-hello`:

  0. `make`
  1. `make mount` will mount the `hello` file system; it will do so
  single-threaded (the `-s` option) and in the fore-ground (the `-f`
  option).  
  2. `make run` (in another terminal).  Will do a trivial test to show
  it is up and running.

This code comes from (https://github.com/libfuse/libfuse/tree/master/example).
There are a bunch of other examples there.  Change the code so
that it prints out:

    hello cs140e: today let us do a short lab!

----------------------------------------------------------------------
## Part 1: Implement FUSE methods to make a simple FS (45 minutes)

If you look in `part1-pi-fs/`:

  - `pi-fs.c`: starter code to implement a simple FUSE file system.  
  The file system just has a single root directory and files, no 
  subdirectories.
  - `pi-fs-support.c`: support code that we provide.
  - `make mount`: will mount your file system.
  - `make unmount`: will forcibly unmount if it gets stuck.
   - `make test`: will execute simple Unix commands to check if your 
   system is broken.

You'll have to implement six methods:

   - `getattr` get attributes of the file: its size, permissions, and
   number of hardlinks to it.

   - `open`: I'd use `file_lookup` in `pi-fs-support.c`

   - `read`: read fro the file at a `offset` of `size` bytes.

   - `write`: write to the file at a `offset` of `size` bytes.

   - `readdir`: return the directory entries in the given directory.

   - `ftruncate`: truncate the file.

Other than `getattr` (which we already defined), these are more-or-less
identical to Unix calls, so you can just man page them to figure stuff
out.  If you run FUSE with debugging output (which we do) you can see
where it is failing and why (usually permission errors).

----------------------------------------------------------------------
## Part 2: Handle redirection

In order to allow your pi-fs to control your pi-shell, you'll have to 
have a way to fork the pi-shell as a sub-process and control its 
input (`stdin`) and see all its output (`stdout` and `stdin`).

You've already done this kind of programming in your `replay` and
`handoff` codes.  With a few small tweaks you can do the same here;
we break this out to make it easier to debug any details in how you
hook the two together.

The main detail to get right is how the redirection process knows when
your shell is done with the current command.  (You'll recall we used
`CMD-DONE` in `lab10` as a hack to figure out when the shell was done.)
Fortunately, this is actually easy (and I should have realized this before):
the shell is done executing a command when it prints its shell prompt.
E.g., `PIX:> `.  So we just need to listen for this.

Rundown:
 - `part2-redirection` holds all the code.
 - `redirect.c:redir` is the routine you need to implement.
 - `driver.c` is the test driver.

You should test from easy to hard:
 - `make run`: tests that you can (1) read `stdout` and `stderr` from
 a subprocess and (2) your code exits when it does rather than hanging.

 - `make run.fake-pi`: checks that you correctly listen for shell 
  prompts.

 - `make run.pi`: checks that you can control your pi-shell.  You pi should
  be plugged in.

----------------------------------------------------------------------
## Part 3: Hook up the your pi-fs to the your pi-shell.

Now you'll combine everything together.

  1.  Copy all your code from part1 into this directory: (`cp ../part1-pi-fs/`\*.[ch] .`).

  2. Implement the `do_echo`, `do_reboot`, `do_run`.

  3. You'll have to use `Makefile.use`

  4. Also, you'll need to replace your main with the code in `patch.c`.
  The easiest way is to put this in your `pi-fs.c`:


    #ifndef PART3
       ...your original main() code....
    #else
    #    include "patch.c"
    #endif


Break this down into steps.

  1. reboot:  `echo 1 > ./pi/reboot` should cause a reboot.  Make sure
  you can do multiple times.
  2. echo: `echo hello > ./pi/echo` should echo the output.  
  3. Get the console working.
  4. Loading a program is a bit annoying.  We didn't think to put a
  size field in the binary, so we don't actually know how big it is.
  We assume all the bytes are there for a write.  This is ridiculous,
  but works for our simple program.  In addition, the shell takes a file
  name to run rather than the bytes of code.  Our hack: write the bytes
  to a file (e.g., `/tmp/hello.bin`) and send this to the pi.

You are done!

----------------------------------------------------------------------
## Further background reading:

Some other places to read:

  1. A reasonable [1-page rundown](https://engineering.facile.it/blog/eng/write-filesystem-fuse/).

  2.  A longer, [old-school rundown](https://www.cs.nmsu.edu/~pfeiffer/fuse-tutorial/html/index.html), with code.

