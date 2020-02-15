### Bootloader prelab

Since this lab is going out so late, there is no official thing you
need to hand in.  HOWEVER, you are strongly encouraged to do all
the prelab work, otherwise it's going to be a long lab --- in particular,
implement the two routines described in Part 3 (below).

### 1. Read through the lab

You'll especially want to look through the protocol in the lab's Part
2 for context.  It's not that complicated, and everything else is only
interesting to the extent it enables you to build the protocol.

### 2. Look around the code.

To save time in lab (yeah, I know, I didn't give you much time to
save time) it's good to get sort-of familiar with the starter code and
project code structure.  Since we are writing code to run on both UNIX
(`unix-side`) and on the r/pi (`pi-side`) we have more directories than
usual for today:


##### `pi-side` directory

The `pi-side` directory holds all the r/pi code.   In it, the 
   - `pi-side/libpi.small` directory: holds a stripped down `libpi` implementation
     called by your pi-bootloader code.  The next few labs will replace
     the `.o` files with your own implementation.

Make sure you look through:

  1. `pi-side/bootloader.c` this holds the starter code that you will extend out
       into a full implementation that receives code from the UNIX side.  

  2. `libpi.small/rpi.h` contains the protypes and descriptions of pi routines
      you (or our starter code) can call.  Each has a comment describing them:
      please post to the newsgroup if these (or any other function descriptions)
      are confusing!   These include routines to read and write memory (do not
      do it directly!), reboot the pi, and send/receive bytes from the UART
      (the hardware on the pi that talks to your TTY-serial device).

It's worth looking at the other files to.  In particular, compare the `start.s` 
here to the `start.s` in your `1-gpio` lab:
   1. What do they even do?
   2. Why are they different?

##### `unix-side` directory

The `unix-side` directory holds all the UNIX side code.  In it, the 
   - `unix-side/libunix` directory holds useful helper routines that
      your UNIX bootloader code will use (as well as subsequent labs).
      These routines are all described in `libunix/libunix.h`.

Make sure you look through:
  1. `simple-boot.c`: this holds the starter code you will extend
     out into a full implementation that sends code to the pi.
  2. `libunix/read-file.c`: you will implement this (see below).
  3. `libunix/find-ttyusb.c`: you will implement this (see below).
  4. `libunix/libunix.h`: this has all the routines you can use.
  5. `libunix/demand.h`: this has a variety of helpful macros to simplify
     error handling / checking.  You can see examples by using `grep` in
     the `libunix` directory.

And, look but DO NOT MODIFY:
  1. `send-recv.[ch]`: this holds helper routines to send/receive data
     to/from the pi.   Use these directly!
  2. `my-install.c`: this is our starter code for setting everything up.

### 3. Implement two Unix support routines.

To get you used to dealing with `man` pages and their ilk, you'll write
two routines need by your bootloader.  Neither is very hard; hopefully
you can do these before lab:

  1. `read_file(size,name)`: read the file `name` into a buffer you
     allocate in its entirety and return it, writing the size of the
     buffer in `size`.  Implementation goes in `unix-side/libunix/read-file.c`.

     Note that you should pad the memory with zero-filled data up
     to the next multiple of 4.  (Easiest hack: just add 4 when you
     allocate it with `calloc`.)

     You'll implement this using `stat()` to get the file size,
     and then use `read()` to read the entire file into memory.
     (Don't use `fread()` unless you set it up to read binary!)
     Don't forget to close the file descriptor!

  2. `find_tty` will look though your `/dev` directory to find the
     USB device that your OS has connected the TTY-USB to, open it,
     and return the resultant file descriptor.  Implementation goes in
     `unix-side/libunix/find-ttyusb.c`

     You should use `scandir` to scan the `/dev` directory.  (`man
     scandir`).  The array `ttyusb_prefixes` has the set of prefixes
     we have seen thus far; if your computer uses a different one,
     let us know!

     You should give an error if there zero or more than one matching
     tty-serial devices.


### 4. Nice haves to make coding faster, better.

Tags: Again, as discussed in the previous lab, please try to get a "tags"
equivalent installed / used on your development environment.  It will make
looking at function / variable / structure definitions much much easier:
you simply will move the cursor to the identifier you care about, do
some kind of key sequence, and your editor will take you to identifier's
definition.  To pick the first google result for `emacs` and `vim`:
 - [tags in vim](https://vim.fandom.com/wiki/Browsing_programs_with_tags).
 - [tags in emacs](https://www.jayconrod.com/posts/36/emacs-etags--a-quick-introduction)


Multiple code windows: the more different contexts you can keep on
your screen, the less you have to keep in your head and the less time
you have to spend hunting around.  There are a variety of different
terminal programs that make it easy to do split-screen and use hotkeys
to jump between them.  I use `terminator` and am happy with it; there
are many others.

`git` conflicts:  you may wind up having a `git` conflict at some point.
The a stupid-simple way if you don't care about your local modifications:

    git checkout -- <file>

See [here](https://docs.gitlab.com/ee/topics/git/numerous_undo_possibilities_in_git/) for
more general variants.

