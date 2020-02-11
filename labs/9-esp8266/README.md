## Simple networking with the ESP8266

Today we're going to do the ESP8266.  

I had big plans to have you implement a remote `PUT32` and `GET32`
that you could then use to control the GPIO on other people's pi's,
both from your pi and from your laptop.

Unfortunately, it has taken a lot longer than anticipated to write a
"simple" parsing library to control the ESP that would work both on the
pi and on your laptop.

So today is going to be free-form.  It's a good exercise in getting a
new device working without a lot of help,  but it's a shame we won't
get to the cooler hacks tonight (fortunately, they will be the homework :)

Deliverables:

  1. You can ping-pong a value with your patner using TCP.  This will
     be the stepping stone for building a `net_GET32` and `net_PUT32`
     needed by the homework.

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

  2. Run `esp-cat` (in `esp-cat/`) and make sure that typing `AT` gives

		AT

        OK
        
### Part 1:  get the ESP8266 working on Unix.

As usual, we try to run faster by taking smaller steps.  Before using the
ESP8266 in the pi, where when we run into bugs it's harder to figure out
what the cause is (bug in your sw-uart?  in how you handle interrupts?
because of memory corruption? timing?) we get it working on your Unix
laptop, where the OS handles buffering and timing issues for us.

We will hook the ESP up to the extra TTY-USB you have.  You can then
run the provided code.   The default firmware on the ESP communicates
with us via the string-based `AT` command languages (you can reflash
the device to use something more sophisticated).   The good thing about
using `AT` is that its human-readable, so its easy to see what is going
on and debug.  The bad thing is that it requires parsing.  This kind of
code can be finicky and tricky, so I've included a brain-dead parsing
"library" to make it easier.  You should poke around to see how it works.

You should get in the habit of using `esp-cat` to figure out what the
ESP replies when you give it various commands.  You can then encode
these in your code.

All of the code is in two directories:
   1. `libesp`: holds the library code for ESP parsing.
   2. `unix-esp`: holds the code for dealing with the ESP on Unix.
   
#### Getting a server up

The code you'll modify is in the `libesp/esp-commands.c`.  You should
build the following so you can run a TCP server:
   1. `esp_setup_wifi`: when this works you should be able to connect to 
      the esp with your laptop.
   2. `esp_start_tcp_server`: to start a TCP server.
   3. `esp_send_hello`: send a hello message.
   4.  Note: all the constants you need are in `libesp/esp-constants.h`.

There's a bunch of helper functions in `libesp`.  However, if you want to 
go free-range you can certainly write your own.   It's unfortunately
trickier than it seems, but as a first step to getting something basic,
it does have an allure.

The `libesp` helper functions are mostly in:
   1. `esp-run.h` --- this is most of the routines.
   2. `esp-parse.h` --- this has some lower level routines.
   3. `libesp.h` --- has structure definitions.

You should be using the examples in `docs/examples-esp8266.pdf` and
the speficiation document to figure this out.  Also, use `esp-cat`
extensively to get the right format and syntax.

When the above work, you can then run

            netcat 192.168.4.1 4444

To see that `netcat` is receiving `hello` messages from the server.

If the esp isn't doing anything, try unplugging and replugging. Also, make
sure your network stays connected.

If this is a hassle, you might try skipping to the next step.

#### Getting a client up

Dealing with laptop wireless connections can be irritating.  So we'll just use
two ESP's to talk to each other.

You should implement:
  1. `esp_connect_to_wifi` to connect to the access point you created previously.
  2. `esp_start_tcp_client` to connect to your server.
  2. `esp_recv` to receive data.

### Homework: the ESP8266 working with the pi with interrupts.

Will write this up this week.  Due two weeks after.

### Homework: build `net_GET32` and `net_PUT32` and control the GPIO on another pi.

Will write this up this week.  Due two weeks after.

### Homework: build `net_GET32` and `net_PUT32` and control your pi from your laptop.

Will write this up this week.  Due two weeks after.
