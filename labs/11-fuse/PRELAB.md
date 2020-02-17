
Fuse has a bunch of moving parts.  You absolutely have to read about it before
lab.

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

To make the reading more goal-directed, start with `0-hello` and 
work through the example. 


