Trying to standardize bullet-proof makefiles that are not a pain to
deal with.

Our current strategy is to make everything depend on everything so I
can get rid of the dependency computation.  The way we computed deps'
in the past caused problems on some student computers for reasons I do
not know (perhaps the substring matching was getting messed up by the
substrings in their path?).  We just get rid of it.  Compilatation is
cheap; lab time is not.

Several approaches:
 - `Makefile.n-file`: if you have more than one program, but it uses
 only one file, copy this and update 

Our two approaches:

 - `Makefile.one-file` can use this to inline everything and specialize make
    for one file (or, really, a string of enumerated `.o` files).   Use this
    by copying it to your directory as `Makefile` and changing `NAME` and/or 
    `OBJS`.

 - `Makefile.example` which uses `template.mk` to make all `.c` files in a given
    directory.  Use this by copying it to your directory as `Makefile` and add any
    target calls you need.

Hopefully should eliminate all breakage during labs.
