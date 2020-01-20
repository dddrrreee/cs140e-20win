Trying to standardize bullet-proof makefiles that are not a pain to
deal with.

Our current strategy is to make everything depend on everything so I
can get rid of the dependency computation.  The way we computed deps'
in the past caused problems on some student computers for reasons I do
not know (perhaps the substring matching was getting messed up by the
substrings in their path?).  We just get rid of it.  Compilatation is
cheap; lab time is not.

If you want things as simple as possible you have two options:

  1. If you want to produce a single executable, copy `Makefile.one-program.caller`
     to your target directory as `Makefile` and change `NAME` to your program.
     It will automatically compile and include all `.c` files in the directory.
     (If you don't want this behavior, use  `template.n-prog.mk` instead).
 
   2. If you want to produce multiple executables, but they only need one
      file each, copy `Makefile.n-program.caller` to your target directory
      as `Makefile` ane change `PROGS` to all the executables you want.

Otherwise:

   - Just copy and modify `template.n-file.mk`.   It has more characters
     in it, but there's no recursion, which can be confusing.

Hopefully should eliminate all breakage during labs.
