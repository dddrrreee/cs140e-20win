## Simple file system.

Last lab we got used to communicating with your pi via messages.  Today 
we hook it up to the FUSE file system so that you can start controlling it
easily from your laptop.

Useful hints:
  1. If you get rid of the `-d` option there is a lot less printed out to the 
     screen.  Can make it easier to debug.
  2. If you get some kind of transport error, do `make unmount`.
  3. To run the fuse command do `make fuse.run` (you might want to modify
     the make target).
  4. Swap in the `main` in `3-fs/patch.c` into your `pi-fs.c`.

Checkoff:

  1.  Your pi-FS works as a standard file system.  `make test` should
      execute as expected.

  2. Change `4-hello-fixed.sys` to use system calls to send characters
     rather than the gross hack we did.  This should be a fairly quick
     repurposing of your lab 7 system call code.

#### Hints

Different things needed for the system calls:
   1. Truncate can both extend (zero-fill) or shrink a file (change its size down).
   2. The `flags` field of the directory has the permission flags needed.
   3. `st_mode` will be `S_IFREG` (for a regular file) bit-wise or'd with 
      the entries `flags`.
   4. `st_nlink` is the number of pointers to this entry.  Since we don't have
      hardlinks, files will have 1 link (their parent dir) and a directory
      will have two (its parent and `.`).  When we add subdirectories, this 
      link count will go up `+1` for each subdirectory (since `..` points to it). 
   5. Both `read` and `write` have an `offset` parameter.  This indicates where
      to start reading or writing in the file.  Reads do not go beyond the end
      of the file, writes will extend the file if need be and, if 
      the offset is beyond the current end of the file, you need
      to zero fill everything up to that point.
  
#### Break things down into pieces.

I'd try simple steps:

  1. First make sure you can handle file writes:

           % echo "hello world." > /pi/echo.cmd
           % cat /pi/echo.cmd
           hello world.
           % echo "bye world." > /pi/echo.cmd
           % cat /pi/console
           hello world.
           bye world.

   2. Then start with just reboot.

           % echo 1 > /pi/reboot.cmd

      Should cause a reboot.

   3. Then make sure you can send code:
   
           % cat hello-fixed.bin > /pi/run.cmd

      Should run as expected.
