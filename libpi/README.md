Main rule:
                        DO NOT MODIFY THE MAKEFILE!
                        DO NOT MODIFY THE MAKEFILE!
                        DO NOT MODIFY THE MAKEFILE!
                        DO NOT MODIFY THE MAKEFILE!
                        DO NOT MODIFY THE MAKEFILE!
                        DO NOT MODIFY THE MAKEFILE!


Also:
  don't add files to src/  or lib/  --- files we add later may conflict with yours.

  however, you can add them to my-src/  --- if you do, the makefile will automatically
  grab them and compile.  this is nice b/c you don't have to do anything.  it's bad
  if you put .c files in there that you don't want compiled.

Basic idea:
  - Put your source code files in this directory.
  - Put their names in the `put-your-src-here.mk` so that the `Makefile`
  will compile them.
  - Don't modify the `Makefile`.

The subdirectories hold `.c` and `.o` files we give you:
  - `cs140e-objs`: these are `.o` files we provide.  you're going to write your own
     and remove these pretty quickly.
  - `src`: these are pi-specific source files.  You can write your own, but 
    these are not super interesting.
  - `libc`: `.c` implementation of standard library functions (often ripped off from
    `uClibc`).

  - `objs`: this is where all the .o's get put during make.  you can ignore it.
