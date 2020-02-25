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
      execute as expected.  When the user writes values to `/pi/echo`,
      `/pi/reboot`, `/pi/run` the relevant command is sent to the pi
      and all pi output is written to `/pi/console`.

  2. Change `4-hello-fixed.sys` to use system calls to send characters
     rather than the gross hack we did.  This should be a fairly quick
     repurposing of your lab 7 system call code.


  2. Your pi-FS works as a standard file system.  `make part0` wil
  run some simple tests.  You should be able to run the usual
  Unix file programs, have them work as expected, and have all
  output appear in `/pi/console`.  For example:

           % echo "hello world." > /pi/echo
           % cat /pi/echo
           hello world.
           % echo "bye world." > /pi/echo
           % cat /pi/console
           hello world.
           bye world.

Extensions:

   1. Add directories.
   2. Add `/pi/dev/led` so that writes turn the led on/off and that
   reads give the current state.
   3. Add a `pthread` thread to your FS that will pull output from
   the pi in the background.

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

----------------------------------------------------------------------
## Part 1: FUSE

##### Look over `0-hello`

You should have already done this for the prelab!  But just to make sure you can 
debug: Run the `hello` example in `lab11-fuse-fs/part0-hello`:

  0. `make`
  1. `make mount` will mount the `hello` file system; it will do so
  single-threaded (the `-s` option) and in the fore-ground (the `-f`
  option).  
  2. `make run` (in another terminal).  Will do a trivial test to show
  it is up and running.

This code comes from (https://github.com/libfuse/libfuse/tree/master/example).
There are a bunch of other examples there.  Change the code so
that it prints out:

    hello cs140e: today let us do a short lab!

#### Implement FUSE methods to make a simple FS 

If you look in `3-pi-fs/`:

  - `pi-fs.c`: starter code to implement a simple FUSE file system.  
     You are more than welcome to start with your hello file system if
     you prefer.  We've defined a bunch of the methods you will need.
     They are likely a bit opeque, so look in the `fuse.h` in your 
     install or in many 

  - your file system just needs a single root directory and files, no 
    subdirectories, so you can do a pretty simple data structure to 
    track this.  Again: you're welcome to kill all of our code.
 
  - `make mount`: will mount your file system.
  - `make unmount`: will forcibly unmount if it gets stuck.
   - `make test`: will execute simple Unix commands to check if your 
   system is broken.

You'll have to implement six methods:

   - `getattr` get attributes of the file: its size, permissions, and
   number of hardlinks to it.

   - `open`: 

   - `read`: read from the file at a `offset` of `size` bytes.

   - `write`: write to the file at a `offset` of `size` bytes.

   - `readdir`: return the directory entries in the given directory.

   - `ftruncate`: truncate the file.

Other than `getattr` (which we already defined), these are more-or-less
identical to Unix calls, so you can just man page them to figure stuff
out.  If you run FUSE with debugging output (which we do) you can see
where it is failing and why (usually permission errors).

## Hook up the your pi-fs to the your pi commands

Now you'll combine everything together.

  1. Implement the `do_echo`, `do_reboot`, `do_run` by using the code from
     part 1.

Break this down into steps.

  1. reboot:  `echo 1 > ./pi/reboot` should cause a reboot.  Make sure
  you can do multiple times.
  2. echo: `echo hello > ./pi/echo` should echo the output.  
  3. Get the console working.
  4. Loading a program is a bit annoying.  We didn't think to put a
  size field in the binary, so we don't actually know how big it is.
  We assume all the bytes are there for a write.  This is ridiculous,
  but works for our simple program. 

You are done!
