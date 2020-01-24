## Today: system call tricks.

To sharpen your intuition about operating systems, this lab is a sort-of
crash course in how to use the most common nouns and verbs in Unix to
do interesting tricks.

We're going to be using various Unix system calls.  Some useful introduction (or
review) reading:
  1. [Lecture 2](https://www.scs.stanford.edu/18wi-cs140/notes/) from Mazieres'
     CS140 lecture on processes.  
  2. [General processes](http://pages.cs.wisc.edu/~remzi/OSTEP/cpu-intro.pdf)
     and the [Process system call API](http://pages.cs.wisc.edu/~remzi/OSTEP/cpu-api.pdf).
     from the useful [Three Easy Pieces](http://pages.cs.wisc.edu/~remzi/OSTEP/#book-chapters)
     textbook.
  3. As always: the man pages.  E.g., `man 2 fork`.


The prelab will have you write two pieces of code:
  1. A short 50-line program that will hopefully help you
     a lot in your later CS work.

  2. A `start_watchdog(int watch_pid)` routine that make it easier to
     develop fork-exec programs (as we do in the lab) and avoid them
     leaving random zombie processes running because of an inopportune
     crash or mistake in your code.

----------------------------------------------------------------------------------
#### Part 0: go through the `useful-examples/` directory

This directory has a variety of small example programs demonstrating how to 
use Unix operations to accomplish  goals related to this lab and others.

You should go through each one and see what it does and how it does it.
I guarantee not skipping through this part will save you a lot of
time later!  Often they are pretty straight-forward.  A few are a bit
opaque, somewhat intentionally, so you have to reason about what is going
on.  There are various bits in the examples that will be useful to steal
for the next lab and some others.

There's a reasonable chance some of these tricks serve you for the next
couple of decades, at least here and there.

----------------------------------------------------------------------------------
#### Part 1: write `cmd-watch`

In my opinion, `sublime` and other IDE's have  crippled how people code --- after
saving a file, often there's 30-60seconds of switching screens, mouse
clicks, hunting for which `foo.c` you want out of many tabs, etc before you
actually get an edit-compile-run done.  I'd assert that you're more likely
to stay in a flow state and get more done if, as soon as you save code,
all the compile-run happens automatically and immediately.

So we are going to build a simple tool that that lets you do so.  It works as follows.
  1. You invoke it:

          cmd-watch ./program-name [arg1 arg2 <...>]

  2. `cmd-watch` will monitor all the `.c`, `.h`, `.S`, `.s` files in the current
     directory and, if they are modified, execute the command `./program-name` with
     any given arguments (e.g., `arg1`, `arg2`, etc.)
  3. For example:

          cmd-watch make

     will run `make` if any of the files `*.[chsS]` change in the current directory.

     This means you can be editing any file, save it, and then ---
     without moving a finger --- the code will be compiled and (depending
     on your makefile) run.  IMO, the best coders have everything they
     do either require one keystroke or (better) zero.  This is a good
     step in that direction.

Most of `cmd-watch` uses similar methods as your `unix-side` bootloader.
You can implement `cmd-watch` as follows:
  1. Uses `opendir` to open the current directory and scan all 
  entries using `readdir`.
  2. For any entry that has the suffix we are looking for, uses `stat` 
  or `fstat` (`man 2 stat`) to get the `stat` structure associated with that entry and check
  the modification time (`s.st_mtime`).
  3. If the modification time is more recent than the last change in the directory, 
  do a `fork-execvp` of the command-line arguments passed to `cmd-watch`.
  4. Have the parent process use `waitpid` to wait for the child to finish and also
  get its status.  The parent will print if the child exited with an error.

Some gotcha's
  - If nothing changed, go to sleep for a human-scale amount,  otherwise
  you'll burn up a lot of CPU time waiting for some entry to change.  The claim
  is that people can't notice below 100ms, so you could sleep for 250ms
  without too much trouble.

  - If you keep opening the directory, make sure you close it after (`closedir`) otherwise you'll run
  out of file descriptors.

  - Even if you find a recent-enough modification time to kick off the
  command, scan the rest of the directory.  Otherwise you can kick off
  spurious executions.

There are a bunch of extensions you can do:
 - Make it easy to use different suffixes or scan in different locations.

 - Run multiple commands, for example `foo \; bar` which will run `foo` and then
  run `bar`.  Note if you use a semi-colon as a separator you will likely need to 
  escape it or your shell will interpret it.

----------------------------------------------------------------------------------
#### Part 2: implement `start_watchdog`

If a parent process `P` forks a child process `C`, and `P` dies, then `C`
can keep running as a zombie.   This is a general issue you have to deal
with when creating processes --- a careless mistake or an inopportune
crash can leave a bunch of them clogging up your system.   This will almost
certainly happen in lab 5.  So, you will build a little routine that will
stop this from occuring.  
   1. After the parent process forks a child process with process id
      `pid`, the parent will call `start_watchdog(pid)`.
   2. `start_watchdog` will fork a watchdog process that will use a pipe
      read to detect when its parent dies (see useful-examples for how you can 
      do this!).  When this occurs, it will use `kill` to terminate the 
      child and then `exit`.

Why this works:
   1. Because the watchdog is in a seperate process, it will not be affected by a 
      parent crash.
   2. Because the `watchdog` is simple, it (hopefully) will not have bugs that 
      lead to our original problem.

The basic idea:
   1. Create a pipe.
   2. Fork.
   3. If parent, return.  Otherwise, in the child:
   3. Close all other file descriptors (for hygiene). 
   4. Do a `read` on the pipe.  When it returns send a kill to the child.
   5. Check the return value of `kill`!   In particular if it has an error it 
      should only be `ESRCH` (see `useful-examples`).
