## Build a simple FAT32 read-only file system.  Use it to fork/exec a process.

***NOTE: Make sure you start with the [PRELAB](PRELAB.md)!***

We will do a fancy hello world: 
  1. Write a simple read-only file system that can read from your SD card.
  2. Use it to read in a `hello-fixed.bin` from last lab.
  3. Jump to it and run this program.

It's kind of cool that we can write a read-only FS for (arguably) the most
widely-used file system out there.  With that said, since we are using a pre-existing
system, there is startup code:
  - `fat32.h`:  this has all of the data structures used by the FAT32 system and
    by the master boot record (you are welcome!).   You should look through this
    since there are all sorts of nifty things.  You already had this from the 
    PRELAB.
  - `fat32-helpers.c`: has various helpers, mostly for sanity checking and
    for printing.
  - `mbr-helpers.c`: has various helpers for the master boot record, 
     mostly for sanity checking and for printing.
  - `fat32-lfn-helpers.c`: this has the code to support "long file names" in 
    FAT32, which is a very gross hack that they did to keep things backwards
    compatible.   I tried to isolate this code.  You are more than welcome to
    write your own version (the wikipedia page is a good place to look).

There's no way around having this startup given how fancy the FAT32 structures
are, so it is what it is.  Fortunately you don't have to look at most of it
and the words for this lab are pretty short.

The tl;dr strategy:
  1. use the annotated copy of Paul's writeup (in the `docs/` directory) and 
     the wikipedia page for further explanation.
  2. Look through `fat32.h`.
  3. Start coding in `driver.c`.
  4. To get checksums, set `trace_p = 1` in `pi-sd.c` and it will emit `TRACE
     messages --- you can grep these out and compare them.

#### Checkoff

You should:
  1. A working sdcard driver that you steal from somewhere else and adapt.
     (This is in the prelab).  Also, make sure you pass the `kmalloc` tests
     in the `PRELAB`.
  2. Read and echo the contents of `config.txt` on your SD card.
  3. You should be able to swap SD cards with your partner and get the same
     output.  (If you add print statements, make sure you have a way to 
     `grep` them out!).
  4. Read in and jump to `hello-fixed.bin` from the `11-fuse` lab (you'll
     have to copy it to your SD card).



You should have something that looks like this at the end:

        ---------------------------------------------------------
        going to concat config.txt
        gpu_mem=64
        
        arm_freq=700
        core_freq=250
        
        kernel=kernel.img
        kernel_address=0x8000
        
        ---------------------------------------------------------
        crc of bootcode (nbytes=50248) = 324c3013

        DONE!!!
        
        Saw done
        
        bootloader: pi exited.  cleaning up

Extensions:
  1. Run the `hello` program in a virtual address range using the last lab.
  2. Make multiple drivers work and cross check they do the same reads
  and writes when you run them (as we did in lab3).


-------------------------------------------------------------------------
### Background: File systems over-simplified.

Hopefully this is redundant with the PRELAB.

Our r/pi's (and most SD card devices) expect to work with Microsoft's
FAT32 file system.  On the plus side, FAT32 is relatively simple and,
because it's so widely-used, fairly-well documented.  On the negative end,
it has a really ugly way to handling filenames longer than 8 characters.
(You can look at the LFN code in the `fat32-helper.c` to get a feel
for this).

The big picture:
   1. From our view, disk is an array.  Each entry is a 512-byte sector.
   The first sector (again from our view, if not from the device's)
   is named 0, the second, 1, etc.  To amortize overhead, devices let
   you read multiple contiguous sectors with a single operation.

   2. Over-simplifying: file systems are trees of files and directories.
   Directories have pointers to files and directories.  Files have
   pointers to data blocks.  For performance and space reasons, most
   file systems do not represent files as contiguous arrays of pointers
   to data (as we would with a simple one-level page table), 
   but instead use an irregular hierarchical structure of
   pointers to different types of structures that eventually point to
   data blocks.  For example, the Unix FFS file system uses an inode to
   hold the initial 12 or so pointers, an indirect block to hold the next
   1024 or so, double indirect blocks, etc.  You've seen these tradeoffs
   and methods in page tables --- the different here is that most files
   are small, and almost all files are accessed sequentially from the
   beginning to the end, so the data structures to navigate them are
   tuned differently.

   3. If we had to only store these trees in in-memory, building a file
      system would be a straightforward assignment in an intro programming
      class.  However, because we have to store the file system on disk,
      file systems were the single largest systems research topic in the
      80s and 90s.  (Mostly b/c each access is/was millions of cycles,
      and you cannot write everything you need atomically.)

      For our lab today, the main issue with disk storage is that 
      pointers will be more awkward since we have to use disk addresses
      instead of memory addresses.  As a result `*p` becomes a bunch
      of function calls and table calculations (similar, for real
      reasons, to what is going on behind the scenes for virtual memory).
      A related issue, is finding the tables --- in our case, the FAT ---
      and understanding what its domain and range is.

### Overview: FAT32.

Our SD cards are broken down as follows:

  - Sector 0 on the SD card: the master boot record (`mbr`, 512 bytes).
    Holds four partition tables giving the disk offset (in sectors)
    where the partition starts and the number of sectors in it.
    For us, the first partition should be as big as the SD card, and
    the rest should be empty.  The partition type should be FAT32.
    (Print and check.)

  - Sector 0 in the partion: the FAT32 boot sector (512 bytes).  FAT32 breaks the
    disk into many fields, some useful ones:
       - Number of sectors in each cluster (`sec_per_cluster`).
       - Total file system size (`nsec_in_fs`),
       - How many sectors the FAT table is stored in (`nsec_per_fat`).
       - `reserved_area_nsec` the number of blocks to skip before reading the 
       FAT table (which is the sector right after the reserved area).

    You can check the boot sector is valid (`fat32_volume_id_check`)
    and print it (`fat32_volume_id_print`).

   - The FAT table itself: read in `nsec_per_fat` sectors starting right after the
     `reserved_area_nsec`.  The FAT table is often replicated back-to-back, so you
     can read in both and check they are identical.

   - The root directory: located by reading `sec_per_cluster` worth of
     data from cluster 2.  

Further reading --- the first three are good summaries, the latter have more detail:

  1. [aeb's summary](https://www.win.tue.nl/~aeb/linux/fs/fat/fat-1.html).
  2. [forensic wiki](https://www.forensicswiki.org/wiki/FAT).
  3. [data structure layout](http://www.c-jump.com/CIS24/Slides/FileSysDataStructs/FileSysDataStructs.html).
  4.  [wikipedia](https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system).
  A full description.  Perhaps more detail than you need, but useful
  for all the weird flags that can pop up.
  5. [linux source](https://elixir.bootlin.com/linux/latest/source/fs/fat/dir.c).


-------------------------------------------------------------------------
### Part 1: An SD card driver.

You should have done this in the PRELAB:

  1. Make sure your SD card reader is working.
  2. It should be able to read in the master boot record (in `driver.c`).  It should pass
     our sanity check `mbr_check`.
  3. Make sure when you run with `trace_p = 1` that your checksum is the same as 
     your partner's when you swap SD cards.

-------------------------------------------------------------------------
### Part 2: read in the volume id (`fat32_boot_sec_t`) and `fsinfo`.  (10 minutes)

This should be fast:
  0. Search for `PART2` in the `notmain` driver.  
  1. Use the MBR to read in the FAT32 boot sector (also called the "volume id").  
     It's the first sector in the 
     partition; there is a structure definition for it in `fat32_boot_sec_t`.
  2. Also read in the `fsinfo` structure.
  3. Both should pass their checks (the code is there)
  4. Make sure you get something sensible: you should be able to swap with your
     partner and get the same results as they did.

-------------------------------------------------------------------------
### Part 3: read in and setup the FAT (20 minutes)

Here you will use the boot record to setup the `fat32_fs_t` structure,
including the actual FAT table.    Use the volume ID and the partition
to implement the `fat32_mk` routine, which will define the important
pieces for your FAT32 FS.

A reasonable description of the information you need is
[here](https://www.pjrc.com/tech/8051/ide/fat32.html)

-------------------------------------------------------------------------
### Part 4: read in the root directory (10 minutes)

You'll get the root directory, read it in, and print it.

   1. Use your fat structure to compute where cluster 2 is (the root directory).
   2. Read it in.
   3. The print loop should print something sensible.
   4. You can also print it with the LFNs (commented out)


*Note: the checked-in code has a completely broken comment that the root
can only be one cluster.*  While I would expect the root directory in
your current SD card to fit in one cluster (since we didn't do much on
the sd card), in general it can be unbounded in size and you will have
to iterate through it, loading its cluster as you do files (below).
The annoying difference from files is that you do not know the size of
the directory up-front.

-------------------------------------------------------------------------
### Part 5: read in and print `config.txt`

If everything has gone according to plan:
   0. You should be able call `dirent_fat32_lookup` to lookup `CONFIG   TXT`
      (the raw name: for an extension you can do better names!).
   1. You should be able to get the cluster id from your directory structure.
   2. Then implement `fat32_read_file` to walk down the fat, reading it into
      an allocated buffer.   
   3. When the entry in the fat is `LAST_CLUSTER` you know you've reached the end.


*NOTE: do not  check the type of the fat entry by hand, in particular do not do 
a raw comparison to `LAST_CLUSTER`, since there are many allowed values for this
sentinel.   Instead, call `fat32_fat_entry_type` which strips out the bits
for you (you can certainly write a more clever version of this routine, 
which you are encouraged to do!):

        // in fat32-helpers.c
        int fat32_fat_entry_type(uint32_t x) {
            // eliminate upper 4 bits: error to not do this.
            x = (x << 4) >> 4;
            switch(x) {
            case FREE_CLUSTER:
            case RESERVED_CLUSTER:
            case BAD_CLUSTER:
                return x;
            }
            if(x >= 0x2 && x <= 0xFFFFFEF)
                return USED_CLUSTER;
            if(x >= 0xFFFFFF0 && x <= 0xFFFFFF6)
                panic("reserved value: %x\n", x);
            if(x >=  0xFFFFFF8  && x <= 0xFFFFFFF)
                return LAST_CLUSTER;
            panic("impossible type value: %x\n", x);
        }

-------------------------------------------------------------------------
### Part 6: read in and run `hello.bin`

   1. Store a `hello.bin` linked to a different address range onto your your SD card
      (use the code from `11-fuse/hello-fixed/`).
   2. Read it into a buffer at the required range.
   3. Jump to it.

-------------------------------------------------------------------------
### Extensions


#### make a simple network file system.

There's an unholy number of extensions you can do for this lab. Maybe the most
significant one is to attach your SD card "file system" to fuse as a 
sort-of network file systems (similar to NFS):
  1. Have a "mount point" on your fake pi file system (e.g., `/pi`).
  2. When FUSE operations are done to files and directories below this mount
     point subdirectory, translate these to messages you can send to the pi 
     and get a response on.
  3.  The pi loop would act on them, and send the results back to your unix
     side, which would forward to the FUSE system.  

For example:
  1. If the user does `cat /pi/config.txt`, forward the
     FUSE `open`, `getattr`, `read` calls to pi by making opcodes (e.g.,
     `PI_FS_OPEN`, `PI_FS_GETATTR`, `PI_FS_READ`), and bundling the the
     arguments passed to FUSE in the messages sent to the pi, (similar
     in spirit, albeit different in details, to how you did commands in
     your `pi-vmm.c` lab).
  2.  The pi side would read the contents of `config.txt` and send it back 
     to the UNIX side.
  3. The unix side would copy these into the buffer based to `pi_fuse_read`.

#### Make your fat32 less primitive.

  1. Instead of dealing with raw names, deal with the human readable ones.
  2. use LFN names.
  3. Handle subdirectories.
  4. Add writes.
  5. Combine the above and then `cp -r` large file system trees from your
     laptop to the pi, and recursively run `cksum` on the contents, making
     sure you get the same values as you get on your laptop.  This is a very
     very hard, very very easy test that will seek and destroy many bugs.
