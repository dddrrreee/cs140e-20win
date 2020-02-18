There's a fair amount of background reading that could be helpful.
Also some amount of simple coding preperation.

So you don't miss it, for coding:

  1. Start with `0-hello` and work
     through the example, you need to figure out why `open`, `readdir`
     `getattr` and `read` act as they do.  (see part 2 below).

  2. Make sure your `my-install` works (see part 3 below).

DO NOT WAIT TO DO THIS TIL LAB.  You will be very far behind.

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

---------------------------------------------------------------------
### 3. modifying and simplifying `my-install` for the lab.

See the [README](0-my-install/README.md).  You'll need to pull over your
simple-boot.c and make it compile.

