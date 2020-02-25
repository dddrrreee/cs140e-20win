## Exporting your pi as a file system.

***NOTE: Make sure you start with the [PRELAB](PRELAB.md)!***

Next lab you're going to use the FUSE file system to mount your pi as
a normal file system on your laptop.   You can systematically expose
different abilities of the pi to the user by mapping the feature to a
special file and potentially overriding the sematics of `read()` and
`write()` with custom actions.

For example:

 - `/pi/reboot`: if you write anything to this file, it will reboot the 
 pi.

 - `/pi/echo`: if you write anything to this file, it will be echoed
 by the pi.

 - `/pi/console`: contains all output from the pi.  

 - `pi/run`: writing a pi binary to this file will cause the pi to run
 it (and the output appended to `pi/console`).

This approach lets you control your pi using standard Unix utilities
rather than writing a custom shell to export different actions.  E.g.,
running `tail -f /pi/console` will cause all output from the pi to 
be echoed to your terminal.

You need two main abilities to do the lab, so we will split it into two parts.
  1. You need to be able to send commands to the pi.    We do this first,
     without using the FUSE file system.  This will make it much easier
     to debug.  You'll do `1-send-code` to send code and then `2-fake-pi`
     do see how to send a variety of commands.

  2. You need to expose these commands via the file system.  After you do 
     the above, it shoudn't be too hard to adapt the the code `2-fake-pi`
     and hook up the FUSE file system.


We will do the first one today.

#### The big picture

Once we wrap the pi up as a set of files, directories, links, then you can
use your shell to work with it as you do any other file system objects.
If you expose enough, you can skip writing pi programs and just write
simple shell scripts.  You now have autocompletion, history, job control,
etc without having to write any code.

Most OO programming --- at best --- allows you to wrap different nouns up
with the same verbs, so that a single piece of code that works on those
verbs can work on any specific instance.  If I could pick one single
success story of this approach, and the one that it simultaneously the
most under-appreciated, it would be the Unix file system interface.

If you can map your thing (e.g., hardware device) to you immediately
have it integrate, fairly cleanly, with all the existing code out there.

Of course, are plenty of things that do not fit the Unix model of
`open()-read()-write()-close()`.  A simple way to get thousands of
examples: `grep ioctl` in Linux device directories and look at all the ---
oft-buggy --- code that was stuffed into these `ioctl`-routines because
it did not fit nicely within the Unix file interface.

### Check-off

You need to show that:
  0. You can run `1-send-code/unix-side/send-pi-prog` and get one of our log files:

        pi-trace my-install ../pi-side/get-code.bin -exec ./send-pi-prog < ../../hello-fixed/hello-fixed.bin
        ls log-files/
        log-file.7115e076.txt

  1. You can run `2-fake-pi/unix-side/2-blink.fake` and get one of our log files.

        pi-replay log.interpose.txt my-install ../pi-side/pi-vmm.bin -exec ./example-pi-programs/2-blink.fake
        ls log-files/
        log-file.2476ee66.txt  log-file.3e1374a4.txt  log-file.87125e4f.txt


----------------------------------------------------------------------
## Part 1: send stuff to the pi.

You already know how to do this for the bootloader, so this is a minor twist.
We do the specific methods  in isolation so we can debug and then do FUSE.

#### send code

If you look in `1-send-code` there is a `unix-side` that sends a program, and 
a `pi-side` that receives it and jumps to it.  

As discussed in the pre-lab, this will differ from the bootloader since
we want to copy code into the address space of a running process (so
can't overlap), jump to it and then return.

If you look in `11-fuse/hello-fixed` you can see the modifications we made:
  1. changed the linker script to link at a non-overlapping address, and 
     stick the address in the binary.
  2. get rid of `rpi_reboot`.

You should implement the code in `pi-side` and `unix-side` and make sure you
can run the code multiple times from the `unix-side` directory:

    make run
    my-install ../pi-side/pi-side.bin -exec  ./send-pi-prog < ../../hello-fixed/hello-fixed.bin

`send-pi-prog` will ship the code over and  exit when its done.  You will rip
this code out later for the FUSE code.

#### remotely control the pi: `2-fake-pi`

We want the FUSE system to send low-level commands to the pi.  We make sure
we can do this by taking a bit of a weird side-step to make a tiny virtual
machine (also with a `pi-side` and a `unix-side`) where the `unix-side` can
send low level commands to the pi and it will execute.  The example code
ships `PUT32` and `GET32` which allows it to run unmodified pi programs.

You should modify this so you can delete the `gpio.c` that it uses and 
instead ships `PI_GPIO_SET_INPUT` etc (see `pi-side/pi-vmm.c` for the
opcodes).  The code in 
   - `2-fake-pi/unix-side/example-pi-programs` has programs that you should
       be able to run.
 
This part helps you learn two additional tricks (albeit in tiny form):
   1. How to make a `libpi-fake` on your own.  It's been a useful trick, but
      since we gave you so much fake code it's hard to see what's going on.
      You'll build your own simple one.
   2. How to find bugs automatically.  People have been burned by a common
      set of errors: not doing a `gpio_set_input` or `gpio_set_output` before
      using a pin, having `gpio_read` return something besides 1 or 0.  However,
      these errors are trivial to catch in a fake enviroment (we can do in the 
      real one too of course).  Just set some state variables (is the
      uart initialzed?) and check them as needed.

You should be able to find all three bugs pretty easily.

##### Specifics: 

Go into `unix-side/example-pi-programs` and make sure you can compile and run

    my-install example-pi-programs/0-reboot
    my-install example-pi-programs/2-blink

Then make sure that making at this level and sending the fake `2-blink.fake`
works (this uses our stuff).

Then comment out `gpio.c` in the Makefile and start building your fake versions.
    
Then work through the rest of the programs, sort of ordered by difficulty:

    example-pi-programs/1-put32.c   
    example-pi-programs/2-blink.c    
    example-pi-programs/3-bug.c  
    example-pi-programs/4-bug.c
    example-pi-programs/5-bug.c

