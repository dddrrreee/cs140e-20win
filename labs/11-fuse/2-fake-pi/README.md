
There are two pieces to this lab:
  1. Sending commands to the pi and getting back results.
  2. Attaching these commands to special files so that you can trigger them
     using normal file system operations.

Since debugging both at once can be confusing in a lab format, we'll do
these pieces seperately and then combine them.

### Sending commands to the pi

We could use the ESP strategy of sending character strings back and forth.
For example, `"reboot"` to reboot.  This approach has the nice feature that
its human-readable, which helps debugging.  Its downsides include:
  1. As we saw with the ESP, parsing code can add up quickly.
  2. It will waste a significant number of bytes over an I/O channel that is 
     already low-bandwidth.

So, instead we will do what adults do for networking, which is to send 
messages in a binary-format.  Each message will get a one-byte opcode,
and any additional data its own structure (we use a structure to help
control layout and help documentation).

A message send is pretty simple:
  1.  send the opcode.
  2.  send the structure.

The pi message receive is:
  1. Spin in a loop, reading a single byte.
  2. Switch on the opcode specified by the byte.
  3. Read any additional data needed by that message.
  4. Process it.
  5. Send a reply (if any).

We have already used roughly this model for bootloading, though there
we wasted a bunch more space and didn't use structures.  We could use the
same exact approach here, but we're trying to take the next step.

### Setup the commands.

I've tried to cut down on the starter code this lab.  simple exec that you run.


Three messages.

each time you read from, it can have messages too.  easier to add a pthreads

do the simplest version:


this will re-affirm what is going on with fake-pi: its not complicated,
but there is enough code in the directory that i think most people
don't know what is going on.

so you'll write a simple version that runs remotely on pi.
   
oh, this is useful.

REBOOT
PRINT_MSG nbytes <string>
GPIO_WRITE [pin] on
GPIO_WRITE [pin] off
GPIO_READ [pin]

gpio_set_input(void) {
}
gpio_set_ouput(void) {
}
void gpio_write_

// do you want to get the output?
void rpi_reboot(void) {
    drain
}

also, support create of a file.


- have them build a ramfs?
