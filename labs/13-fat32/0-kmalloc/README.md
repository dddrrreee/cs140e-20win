Some simple checks that your `kmalloc` behaves as expected:
  1. It always rounds up to 8.
  2. `kmalloc_aligned` works.
  3. `kmalloc_init_set_start` works.

The tests are not rigorous, so this is not a verification!
