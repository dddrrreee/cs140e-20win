---
layout: page
title: Quick overview.
show_on_index: true
---

## Lab: build a bootloader.

***NOTE: Make sure you start with the [PRELAB](PRELAB.md)!***

The next few labs attempt to remove all magic from your pi setup
by going downward and having you write and replace all of our code.
The only code running when you boot (and the only code you run to boot)
will either be stuff you wrote, or simple library routines we give you
(which you can also replace).

Today's lab is a big step in that direction.  You will:

  - Write your own bootloader (both the Unix and pi side) to replace
     what we used last lab (i.e., `pi-install` and `bootloader.bin`).

In the next two labs you will write your own versions of the low-level
device code your bootloader needs (some fancier GPIO functions and a
UART device driver).

### Sign off.

  1. Show you your bootloader works by running run `./my-install hello.bin` in 
     the `unix-side` directory.

  2. Swap SD cards with your partner and show that (1) still works.

  3. Show us your code so we can check for some common mistakes.

### Step 1: send `hello world` from pi to Unix

If you can't reliably send and receive bytes on either side, the bootloader
can't work, so we verify you can do this by sending a `hello world` from the
pi to unix.

The key files (as discussed in the PRELAB):

  1. The sub-directory `pi-side` has the skeleton code
     that will run on the pi.  In this part and the next, you'll write
     missing code in `bootloader.c:notmain`, compile it, and copy 
     the resultant `kernel.img` to the SD card just as you did in the
     previous lab.

  2. The sub-directory `unix-side` has the code that will run on
     your Unix laptop. You'll implement the code missing in
     `simple-boot.c:simple_boot`. 

For this part:

  1. Change `pi-side/bootloader.c:notmain` to use `putk` to
     send `hello world` to the Unix side and then `get_uint` to wait
     for a 32-bit magic value (e.g., `0x12345678`) to be sent back.
     Print whether it matched or not, `delay_ms(500)`, and reboot.

  2. Change the code in `unix-side/simple-boot.c` to wait for a
     print message and then send the magic value above back.  To run 
     you'll need to supply a dummy binary, even though you are not 
     sending it:  `./my-install hello.bin` should work.

At this point you should be in fine shape to complete start implementing
your bootloader (below).

### Step 2: send `hello.bin` from UNIX to the pi.

Here you'll write implement the code to:
  1. pi: ask for the program to run;
  2. unix: send the program code;
  3. pi: receive the program code, copy it to where it should go, and jump to it.

A key feature you have that last year's students did not is the
ability to use `putk` from your bootloader code.   This makes debugging
wildly easier.  (Embarrassingly, I only realized the trick to allow easy
printing this past week, the surgery that ensued is why the lab is late.)
Without output, all bugs lead to: "my pi isn't
responding," which is difficult function to invert back to root cause.

I would use `putk` judiciously as you incrementally develop the code so
you can see what state the pi believes it is in.

#### A (typical) nasty hardware limitation

A key limit of the pi UART hardware is that it only has space to hold 8
bytes in its internal hardware buffer:  if your pi code take too long
between UART reads, the hardware will fill up and then drop bytes.
Thus, whenever the pi side is expecting to receive data, it has to
move promptly.

If you forget this limitation, and (for example) print using `putk`
when a message is arriving, you'll almost certainly input lose bytes,
and also get confused.  Ask me how I know!

#### A strong coding hint (PLEASE DO THIS!)

For this part you're writing two pieces of code that talk to each other
through messages.  If at all possible I would strongly suggest you figure
out how to have two code windows side-by-side, one holding the pi code,
the other holding the unix code.  This allows you to easily check that
the bytes you send in one have a matching set of receives on the other.

With two coding windows, if you have a good grasp of the protocol below
and of the helper functions, you may be able to type all this out in
about 10-20 minutes.  If not, it could take hours.

#### The bootloader protocol.

This is a stripped down version (explained more below):

     =======================================================
             pi side             |               unix side
     -------------------------------------------------------
      put_uint(GET_PROG_INFO)+ ----->

                                      put_uint(PUT_PROG_INFO);
                                      put_uint(ARMBASE);
                                      put_uint(nbytes);
                             <------- put_uint(crc32(code));

      put_uint32(GET_CODE)
      put_uint32(crc32)      ------->
                                      <check crc = the crc value sent>
                                      put_uint(PUT_CODE);
                                      foreach b in code
                                           put_byte(b);
                              <-------
     <copy code to addr>
     <check code crc32>
     put_uint(BOOT_SUCCESS)
                              ------->
                                       <done!>
                                       start echoing any pi output to 
                                       the terminal.
     =======================================================

More descriptively:

  1. The pi will repeatedly signal it is ready to receive the program by 
     sending `GET_PROG_INFO` requests.  (This is given in the starter code.)

     (Q: Why can't the pi simply send a single `GET_PROG_INFO` request?)

  2. When the unix side receives a `GET_PROG_INFO`, it sends back
     `PUT_PROG_INFO` along with: the constant to load the code at (for
     the moment, `ARMBASE` which is `0x8000`), the size of the code in
     bytes,  and a CRC (a collision resistant checksum) of the code.

     (Q: Why not skip step 1 and simply have the unix side start first
     with this step?)

     Since the pi could have sent many `GET_PROG_INFO` requests before
     we serviced it,  the Unix code will attempt to drain these out.
     Also, when the Unix code starts, it discards any garbage left in
     the tty until it hits a `GET_PROG_INFO`.

  3. The pi-side checks the code address and the size, and if OK (i.e.,
     it does not collide with its own currently running code) it sends a
     `GET_CODE` request along with the CRC value it received in step
     2 (so the server can check it).  Otherwise is sends an error
     `BAD_CODE_ADDR` and reboots.

  4. The unix side sends `PUT_CODE` and the code.

  5. The pi side copies the received code into memory starting at the
     indicated code address (from step 2).  It then computes
     `crc32(addr,nbytes)` over this range and compares it to the
     expected value received in step 2.  If they do not match, it sends
     `BAD_CODE_CKSUM`.  If so, it sends back `BOOT_SUCCESS`.

  6. Once the Unix side receives `BOOT_SUCCESS` it simply echoes all
     subsequent received bytes to the terminal.

  7. If at any point the pi side receives an unexpected message, it
     sends a `BOOT_ERROR` message to the unix side and reboots.
     If at any point the Unix side receives an unexpected or error
     message it simply exits with an error.

The use of send-response in the protocol is intended to prevent the
Unix-side from overrunning the pi sides finite-sized UART queue.
The CRC and other checks guard against corruption.

I would start small: do each message, check that it works, then do
the next.  If you go in small, verifiable steps and check at each point,
this can go fairly smoothly.  If you do everything all-at-once and then
have to play wack-a-mole with various bugs that arise from combinations
of mistakes, this will take awhile.

And don't forget: there are a bunch of useful error checking macros in
`libunix/demand.h` and uses in `libunix/*.c`.

#### Done!
