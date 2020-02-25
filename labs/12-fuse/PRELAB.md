You should do the readings from last lab, otherwise FUSE won't make much sense.

To help with this, look through the code in `0-examples`:
  - `bbfs.c` has extensive comments.  
  - `fuse.h`: the FUSE header file with comments on each operation.
  - `fuse-examples`: examples from the FUSE `git` repo.
  - `examples/fusexmp_fh.c`: a more worked out example from the macOS distribution.

You'll need to do some simple changes to part 1 and part 2 from last
time to make the system easier to build with FUSE.  After doing each,
make sure your code works!

=========================================================================
## 1. code changes to hello-fixed and 1-send-code

It turns out it's much easier to send programs from FUSE if the binary contains
the program size in it.  It's also easier to add stuff to the header later if
we include the header size (otherwise we have to change every piece of code
that deals with our pi binary).

I changed `hello-fixed/memmap.fixed` to have:
   1. The header size as the second word.
   2. The size of the code as the forth word.
   3. (1) + (2) = the total binary file size.
   4. The CRC sent during loading is over the entire file (both header and
      code) not just the code.

You can look at `11-fuse/hello-fixed/memmap.fixed` to see:

    LINK_ADDR = 0x100000f0;
    SECTIONS
    {
        /* our simple a.out header.  will add as needed. */
        .crt0_header LINK_ADDR : {
            /* magic cookie for sanity checking */
            LONG(0x12345678);
    
            /*
            * the second word is the size of the header.  this lets us avoid changing
            * a bunch of code as we add fields.  You should copy the entire header
            * and just jump over it.
            */
            LONG(SIZEOF(.crt0_header));
    
            /* Where we are linked at */
            LONG(LINK_ADDR);
            /*
            * total size of the binary file in bytes not including the header.
            * NOTE: bss data is *not* in the .bin: it contains only zeros so we
            * do not need to record them, just its size.
            */
            LONG(SIZEOF(.text) + SIZEOF(.data) + SIZEOF(.rodata) );
    
            /* can add stuff below and loader should still work */
        }
    

Since the program header has grown from 8 bytes to 16, you'll have to change
both your pi and unix side.   My unix side looks like:

        char buf[1024 * 64];
    
        // defined in pi-message.h
        struct pi_bin_header h;
    
        read_exact(0, &h, sizeof h);
        assert(sizeof h == h.hdrsize);
        uint32_t expect = 0x12345678;
        if(h.cookie != expect)
            panic("invalid cookie: %x, expected %x\n", h.cookie, expect);
    
        output("going to load code at 0x%x\n", h.addr);
        // make sure it won't hit our pi code.
        if(h.addr <= HIGHEST_USED_ADDR)
            panic("specified addr %x will hit pi code %x\n",
                    h.addr, HIGHEST_USED_ADDR);
    
        int n = read(0, buf, sizeof buf);
        if(n < 0)
            sys_die(read, weird error);
        if(n == sizeof buf)
            panic("overflowed buffer: increase from %lu bytes\n", sizeof buf);
        if(n != h.nbytes)
            panic("invalid size: read %d bytes, expected %d nbytes\n", n, h.nbytes);
    
        uint32_t crc = our_crc32_inc(buf, n, our_crc32(&h, sizeof h));

=========================================================================
## 2. code change to pi-vmm

The main change from last time: it turns out to be much easier to do certain
operations if we can tell the pi vmm to send a message back when it completes
the operation.  How:
  1. Bit-wise or in `PI_NEED_ACK` with any message opcode that does
     not already have a reply.  For example, `PI_PUT32`.   (I believe)
     you should ignore the `PI_NEED_ACK` if opcodes that returns a reply
     already, such as `PI_GET32`.

  2. The vmm loop checks for this, and if it is present, sends back a `PI_READY`
     when it completes.

There are a couple of locations to change.   On the unix side:

    void clean_reboot(void) {
         output("about to exit\n");
        // we don't have to wait?
        put_uint8(PI_REBOOT | PI_NEED_ACK);
        expect_op(PI_READY);
        output("done!  got a ready\n");
        exit(0);
    }
    
On the pi side, any opcode that does not already reply should check:

    // send a PI_READY if opcode has PI_NEED_ACK in it.
    static void send_ack(unsigned op) {
        if(op & PI_NEED_ACK)
            put_byte(PI_READY);
    }


        // notmain message loop: do a send_ack(op) for any opcode that
        // does not already do a reply.
        op = get_byte();
        switch(op & ~PI_NEED_ACK) {
        case PI_READY:
            put_byte(PI_READY);
            break;
        case PI_REBOOT:
            printk("PI: reboot\n");
            uart_int_flush_all();
            send_ack(op);
            delay_ms(100);  // let the hardware actually flush
            rpi_reboot();   // do a silent reboot so we don't generate more prints!

        /* ... */

        case PI_PUT32:
            addr = get_uint32();
            val = get_uint32();
            PUT32(addr,val);
            send_ack(op);
            break;



In addition, to make debugging easier, you should change your `expect_op`
to keep printing whatever it gets from the pi after an error, otherwise
you'll get mysterious messages and not have any context.

    static void expect_op(uint8_t expect) {
        uint8_t got = get_op();
        if(got == expect)
            return;
        output("expected %d, received %d ('%c')\n", expect, got, got);
        // dump out the rest of the string so we can see what is going on.
        while(1)
            output("%c", get_op());
    }


=========================================================================
### 3. Changing trace in 5-replay

As we send more and more stuff between the  pi and unix it's more and more 
useful to check that everyone sends the same thing.

Unfortunately, when I did lab 5 I got too cute with command line argument parsing
so we have to change the code so we can do more complicated versions.

As always, when we make changes that should not modify old results, we want to 
get the old checksums.  So, before you do the modifications below to handle
argument passing make sure you get the checksums:

  1. `cd 5-replay/code`
  2. `make trace`
  3. `./trace my-install ./hello.bin
  4. record the checksum (I got: `log-file.9d10342e.txt`).


If you change `5-replay/code/trace.c`:

    // trace.c
    int main(int argc, char *argv[]) {
    #if 1
        char *bootloader = argv[1];
        char **argv_rest = 0;
        char *dev_name = 0;
    
        if(argc < 3)
            die("ERROR: wrong number (n=%d) of arguments to %s\n", argc, argv[0]);
        else if(strncmp(argv[2], "/dev", 4) != 0) {
            argv_rest = &argv[2];
            dev_name = find_ttyusb();
        } else {
            if(argc < 4)
                panic("too few arguments\n");
            dev_name = argv[2];
            argv_rest = &argv[3];
        }
    #else
        char *bootloader = argv[1];
        char *pi_prog = argv[argc-1];
        char *dev_name = 0;
    
        // interpose my-install foo.bin
        if(argc == 3)
            dev_name = find_ttyusb();
        // interpose pi-install /dev/ttyUSB foo.bin
        else if(argc == 4) {
            dev_name = argv[argc-2];
            const char *prefix = "/dev/";
            if(strncmp(dev_name, prefix, strlen(prefix)) != 0)
                die("ERROR: unexpected device=<%s>, expected a <%s> prefix\n", dev_name, prefix);
        } else
            die("ERROR: wrong number (n=%d) of arguments to %s\n", argc, argv[0]);
    #endif
    
And `interpose.c`:

    endpt_t start_unix_side(char *prog_name, char **argv_rest) {
    #if 1
    #   define MAX_ARGS 12
        char * argv[MAX_ARGS];
        memset(argv, 0, sizeof argv);
    
        argv[0] = prog_name;
        argv[1] = "/tmp/fake-dev";
        for(int i = 0; argv_rest[i]; i++) {
            assert((i+2)<MAX_ARGS);
            argv[i+2] = argv_rest[i];
        }
    #else

        char * argv[4];
        argv[0] = prog_name;
        // so it does not go search for one.
        argv[1] = "/tmp/fake-dev";
        argv[2] = pi_prog_name;
        argv[3] = 0;
    #endif

And finally `interpose.h`:

    endpt_t start_unix_side(char *prog_name, char **argv_rest);

#### replay

Replay is a two line change.

Comment out:
    //char *pi_prog = argv[3];


Change the `start_unix_side`:

        endpt_t u = start_unix_side(unix_side, &argv[3]);
