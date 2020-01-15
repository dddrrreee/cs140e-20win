#### Deliverables before class.

(You don't have to turn these in: but you should do them before
lab so we aren't stuck there til 11pm again!)

To refresh your memory, re-read the Broadcom GPIO descriptions that we
read in lab 1.  Ideally implement `gpio_set_function` (described below).

Before class, make sure you can 

  0. Compile `libpi` (this requires setting an environment variable, 
     described below).

  1. Compile `3-cross-check/hello` and make sure it will run when you
     send it to the pi.

  2. Then after 1 and 2, put your `gpio.c` into the `libpi` library
     directory.  Edit `gpio.h` to include `rpi.h` and also
     edit `put-your-src-here.mk` to use `gpio.o` and not use
     `cs140e-objs/gpio.o`.  

     Make sure any routine that takes a `pin` simply returns if the pin
     value is greater than 31:

         if(pin >= 32)
            return;

     (Anyone making the argument that we should return an error has the
     better end argument, but for the moment we live in sin.) 

     Make sure the result compiles.

-------------------------------------------------------------------------
#### 1. Re-read the GPIO prose in the Broadcom document.

Today you're going to write a `gpio_set_function`:

    // libpi/src/gpio.h
    enum {
        GPIO_FUNC_INPUT   = 0,
        GPIO_FUNC_OUTPUT  = 1,
        GPIO_FUNC_ALT0    = 4,
        GPIO_FUNC_ALT1    = 5,
        GPIO_FUNC_ALT2    = 6,
        GPIO_FUNC_ALT3    = 7,
        GPIO_FUNC_ALT4    = 3,
        GPIO_FUNC_ALT5    = 2,
    };

    // set <pin> to have the function given by <func>
    int gpio_set_function(unsigned pin, unsigned func);

This will set any of the function values for a GPIO pin.  Re-read the
Broadcom description for `GPFSELn` starting on page 91 (you already have,
so this shouldn't take long).  This routine will look fairly similar to
`gpio_set_input` except what it sets the bits to will be an argument.

-------------------------------------------------------------------------
### 2. Setting up your `libpi`

We're going to start scaling up your programs and generally build on
each part of the code you write.    These parts will be collected in
a library, located in `cs140e-20win/libpi`.

#### Setup your  CS140E_PATH variable.

Similar to how you added your local `~/bin` directory to your
shell's path on the first lab, today you're going to edit your
shell configuration file (e.g., `.tcshrc`, `.bash_profile`, etc)
to set set an environment variable `CS140E_PATH` to contain where
you have your repository.

E.g., for me, since I'm running `tcsh` I edit my `.tcshrc` file and
add:

    setenv CS140E_PATH /home/engler/class/cs140e-20win

To the end of it.  If you use `bash` you'll do an:

    export CS140E_PATH=<path to your repo>

And, as before, don't forget to source your configuration file.


#### Make sure libpi compile

If you change directories into:
   1. `cs140e-20win/libpi` and type `make` it should compile without issue;
   2. Into `cs140e-20win/labs/3-cross-check/hello` and type `make`, it 
      should compile without issue.
   3. Dependencies should work --- if you `touch rpi.h` in `libpi` and do 
      a `make` in `hello`, everything should recompile.  You should never
      have to type `make clean` then `make` after making a change!

#### look around at libpi

Look around the `libpi` code.  Most of it are simple library functions
to save you time.

The subdirectories in `cs140e-20win/libpi`:
  - `src`: these are pi-specific source files.  You can write your own, but
    these are not super interesting.

  - `libc`: `.c` implementation of standard library functions (often
    ripped off from `uClibc`).   Other than the printing functions,
    the code here is generally pretty clean and is worth a look.

  - `cs140e-objs`: these are `.o` files we provide.  you're going to
    write your own and remove these pretty quickly.

  - `objs`: this is where `make` puts all the .o's; you can ignore it.


-------------------------------------------------------------------------
#### Cross checking background

One way to compare two pieces of code A and B is to do so by just
comparing the code itself.    This is what you've been doing so far
in class: when your code doesn't work, you'll sometimes stare at
someone else; or when it does, you'll stare to verify you both did
the same.  This can be a fine approach.

If two pieces of code are identical, then we can trivially do this
automatically using something like `diff` or `strcmp`.  However, if the
code differs in any way besides mere formatting, then a simple string
comparison fails.

The other approach is to just run tests, and compare the test output.
A nice result of this approach is that even if two pieces of code
look wildly different we can still automatically detect when they are
equivalant by comparing their outputs.  This is why your classes use
it for homeworks.

The better your testing, the more chance you have of finding where
the code differs.  The intuition here is that we don't actually care
how the code works, just that it has the same end result --- i.e., its
"side-effects" are the same.  In general, side-effects include anything
code does visible to the outside world: its writes to memory, what it
prints, any network packets it sends, etc.

We're going to do a special case version of this check, since for the r/pi
code so far, all we care about are reads and writes to device memory.
If we record these, and compare them to another implementation, if
they read or write the same memory in the same order, we know they are
the same.  (Later, when we get more advanced, in some cases they can
read or write in different orders, but we ignore this for now.)

A drawback of this approach that we have glossed over, is that we
only check equivalance on the inputs we run them on, so to really show
equvalance we need to exhaust these.  Fortunately, if you look at the
r/pi code we have written so far, it has simple inputs --- either none
(where it just reads/writes device memory to initialize it) or a single
pin input, which we can more-or-less exhaustively test (`0..31` and some
illegal values).

