### start of a simple user space testing library

Put your `fake-put-get.c` in this directory.

This library will build up additional code we need to run pi programs on a fake
Unix environment for testing.


Currently:
  
  - `pi-random.c`: a ripped off version of `random` found in most
  UNIX `libc` implementations.  While the fact that `random` returns
  a pseudo-random number is defined in the standard, its the exact
  implementation is not.  Since our testing code uses `random()`
  extensively, differing versions would lead to spuriously different
  comparisons.  Thus, we include our own version. 

  - `pi-test.c`: a pretty bare implementation that includes a minimal
  amount of infrastructure to run our currently trivial pi programs in 
  user-space on most Unix.
