## Simple networking with the ESP8266

Today we're going to do the ESP8266.  

The `1-ping-pong` lab has a client and a server program.  You should be able to
plug in two esp's and run the server using one tty and the client on the other.
   1. start there server first.
   2. after it is waiting for data, then start the client.
   3. they should send hello messages back and forth.
   4. There are logs in the directory after each run.  You can also switch
      between input options in `libesp/esp-lex.h` which gives and example
      of using `ifdef` to switch between implementations.

Deliverables: today is kind of a light lab:

  1. You can ping-pong a value with your patner using TCP.  This will
     be the stepping stone for building a `net_GET32` and `net_PUT32`
     needed by the homework.

  2. You can run the sw-uart.

  3. Do something interesting with the esp (below).

### Part 0: Make sure your esp8266 works

Before getting into the interrupt code, we will just debug that the
`esp8266` device you have actually works.  

Before connecting, the most important rule for connecting the esp8266
to either your tty-USB device or your pi:

   ***MAKE SURE YOU ONLY CONNECT TO THE 3V (NOT THE 5V)***

   ***MAKE SURE YOU ONLY CONNECT TO THE 3V (NOT THE 5V)***

   ***MAKE SURE YOU ONLY CONNECT TO THE 3V (NOT THE 5V)***

   ***MAKE SURE YOU ONLY CONNECT TO THE 3V (NOT THE 5V)***

   ***MAKE SURE YOU ONLY CONNECT TO THE 3V (NOT THE 5V)***

It will fry, and I only have so many of them.

How:

  1. Hook up the esp8266 to your tty-USB and plug it into your laptop.

  2. You should be able to run the server code above and see stuff going out.
        
### Part 1:  make a copy that does PUT32 and GET32

The code you'll modify is in the `libesp/esp-commands.c`.  First make
sure the ping pong works.  Then start generalizing it.

Copy the ping pong code to a new directory:
  1. Change it so you can do a `PUT32` and a `GET32` and the values are
     correct back and forth.

  2. Measure the overhead.

### 2. realism.

Start adding functionality to your `libesp`.   Please keep all your code in
`esp-commands.c` so that I can push any bug-fixes without conflicts.

  1. Try to handle mutiple client connections.  this will require you
     can accept `CONNECT` and `DISCONNECT` messages during normal message
     processing --- you will need to modify the out-of-order messages handler
     to do this.   Connect from multiple esps.

  2. Handle errors.  for the client, write the code to do a close and modify the 
     server to handle it.  Also, have the client reboot (versus getting stuck) if the
     server goes away.

### Homework: the ESP8266 working with the pi with interrupts.

I checked  in a hardware `cs140e-objs/uart-int.o` --- add this to your
`put-your-src-here.mk` and run the example to make sure things work.
You can start writing the `pi-support.c` code in `libesp` and getting the 
code to run.

Will write this up this week.  Due two weeks after.

### Homework: build `net_GET32` and `net_PUT32` and control the GPIO on another pi.

When you have the above working, make network versions of the `GPIO` routines
and control another pi.

### Homework: build `net_GET32` and `net_PUT32` and control your pi from your laptop.

Will write this up this week.  Due two weeks after.
