We won't be using FUSE tomorrow.  You should pause that lab and do the reading below
or the wednesday lab is going to be rough:

  - [aeb's summary](https://www.win.tue.nl/~aeb/linux/fs/fat/fat-1.html).
  - [data structure layout](http://www.c-jump.com/CIS24/Slides/FileSysDataStructs/FileSysDataStructs.html) this has useful tables.
  - [paul's summary](https://www.pjrc.com/tech/8051/ide/fat32.html).  This has
    some useful pictures.
  - [cpl.li's writeup](https://cpl.li/2019/mbrfat/) -- more pictures.  Some
    extra sentences.
  - [OSdev's partition writeup](https://wiki.osdev.org/Partition_Table).
  -  [wikipedia](https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system).
     full description.  Perhaps more detail than you need, but useful
     for all the weird flags that can pop up.
  - [linux source](https://elixir.bootlin.com/linux/latest/source/fs/fat/dir.c).
  - The file system implementation chapter from the "three easy pieces" book 
    (in the `docs/` directory).
  - Microsofts specification (in the `docs/` directory).
  - [forensic wiki](https://www.forensicswiki.org/wiki/FAT) --- seems to be gone.

I'm going to redo some hints for FUSE after tomorrow.

--------------------------------------------------------------------------
### Check your kmalloc works.

The directory `0-kmalloc` has a simple test of tests for your kmalloc.  You
need to have:
  - `kmalloc_aligned`
  - `kmalloc_init_set_start` (to set where the heap starts).

Make sure:
  - you always return an 8-byte aligned pointer if no alignment is specified.
  - Zero fill memory.
  - Don't over allocate.

You should be able to test with:

    make
    make check

If your shell barfs on `make check` you'll have to run the commmands manually
and compare to `out.ref`.

--------------------------------------------------------------------------
### Get an SD card driver.

In general, we prefer to write our own code from primary sources
(datasheets, ARM manuals):
  1. Otherwise you really don't know what you're talking about and (related);
  2. Whenever we do so, we realize how broken most of the other code out there is.

However, for larger drivers, we'll lower our standards and steal other
people's code.   This is a useful thing to do even if you are going to
write from scratch so that you can cross-check against a purportedly
working implementation.  (A good project for the class would be building
a driver cross checking framework --- I can pretty much guarantee there
are different tricky behaviors in the ones out there.)

One useful ability is being comfortable ripping code out of its original
source-tree and making shims to have it work in yours.  Today's lab will
be good practice for doing so.

   1. Go to 
      [bzt's repo](https://github.com/bztsrc/raspi3-tutorial/tree/master/15_writesector)
      and get `sd.c`, `sd.h` and `gpio.h`.

   2. Put these in the `0-sd-driver/external-code` directory, rename them with 
      a `bzt-` prefix (`bzt-sd.c`, `bzt-sd.h`, `bzt-gpio.h` --- update any includes).

   3. Make them compile on our code.

   4. You'll have to `#define` various functions: `wait_msec` `uart_puts` and 
      `wait_cycles` (I put these all in `sd.c`)
      but there shouldn't be much difference.    In general, we prefer
      external changes versus changing the code directly so that we minimize
      the work if we want to pull updates from the original code base.

   5. This code is for the 64-bit rpi3 so you'll get a few warnings about
      printing variables; you can comment these two cases out.  You will 
      definitely have to change where it thinks the base of the GPIO memory region
      is (we use
      `0x20000000`).  If the code hangs, it's because this base was not set.
      (This can be a tricky error to figure out, so remember this fault mode
      for when you port code from other r/pi models to yours.)

   6. You'll also get undefined reference to a division function.
      If you look at the code, you'll notice the division is by a constant
      passed as a function argument, which is good news
      because it lets us eliminate the problem by simply declaring the 
      routine as `inline` (`gcc` will replace the division by shifts, 
      adds and possibly multiplications --- you can examine the `.list`
      file to check).  

      If you are having a hard time finding the 
      division, run :

              arm-none-eabi-objdump -d external-code/bzt-sd.o >x

      and look for the calls to division.

The helper routine `sec_read` `driver.c` uses the code you imported.
It allocates a buffer needed to hold `nsec` worth of data, read the data
in, and return a pointer to the buffer.

After 10-20 minutes you should have a working driver.  When you run it on
your pi, the master boot record should check out.  You can start poking
at other data structures too.
