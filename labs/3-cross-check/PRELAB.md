#### Deliverables before class.

To refresh your memory, re-read the Broadcom GPIO descriptions that
we read in lab 1.  Especially the part for `GPFSELn` on page 91.
Ideally implement `gpio_set_function` (described below).

Before class, make sure you can 

  0. Compile `libpi` (this requires setting an environment variable, 
     described below).

  1. Compile `3-cross-check/hello` and make sure it will run when you
     send it to the pi.

  2. Then after 1 and 2, put your `gpio.c` into the `libpi` library
     directory.  Edit `gpio.h` to include `rpi.h` and also
     edit `put-your-code-here.mk` to use `gpio.o` and not use
     `cs140e-objs/gpio.o`.  Make sure the result compiles.

#### 1. Re-read the GPIO prose in the Broadcom document.

Today you're going to write a `gpio_set_function`:

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

### 2. setting up your `libpi`

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
   1. Your `libpi` and type `make` it should compile without issue;
   2. Into `labs/3-cross-check/hello` and type `make` `hello` should compile.
   3. Dependencies should work --- if you `touch rpi.h` in `libpi` and do 
      a `make` in `hello`, everything should recompile.

#### look around at libpi

Look around the `libpi` code.  Most of it are simple library functions
to save you time.

The subdirectories:
  - `src`: these are pi-specific source files.  You can write your own, but
    these are not super interesting.

  - `libc`: `.c` implementation of standard library functions (often
    ripped off from `uClibc`).   Other than the printing functions,
    the code here is generally pretty clean and is worth a look.

  - `cs140e-objs`: these are `.o` files we provide.  you're going to
    write your own and remove these pretty quickly.

  - `objs`: this is where `make` puts all the .o's; you can ignore it.
