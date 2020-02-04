must have for today:
    - rewrite timer
    - add symbols
    - add uart interrupt.
    - add sys call
    - add break

- kmalloc offsets
- thread offsets.

ugh.


For this lab, we're going to do interrupts and use them to build a
statistical profiler.  The lab code is going to get rewritten extensively
to make it simpler so will change a bunch --- don't modify it yet.

This prelab is going to get filled out more --- so check it out more later.
You should definitely make
sure to read the two pdf's in the `7-interrupts/docs`:
  - `BCM2835-ARM-timer-int.annot.pdf` --- excerpt from the Broadcom document,
     discusses how to enable both general and timer interrupts.

   - `armv6-interrupts.annot.pdf` ---  excerpt from the ARMv6 document in 
     our main `doc` directory.  Discusses where and how interrupts are delivered.
     You should figure out where the handler lives, and registers are written
     what values, and how to restore them.

Code:
    - do find_usb_first (my-install), find_usb_last (pi-install)

    - save all registers into the offset that ='s name.
    - save onto stack.
    - write code to nm, concat to end of binary.
    - write the code to load.
    
    - backtrace: go one back.  useful.  can go back one pretty easily.
      i'm not sure the right way to do this  --- 
        to go back one, need to look at if it's a push, or if it's not
        (leaf?) and you can just use the lr.

        nice example of how to disassemble code.

for someone who goes on and on about cross-checking, i keep mising a bunch of places,
which has led to people having some nasty bugs.  (dealing with such things is a 
very system's talent, but still)

    - kmalloc: add kmalloc_align.  run the test.  this checks that you alloc the
    same blocks in the same order.

    - add traces to the threads, which says which thread running with which stack pointer.

    - then change to using a single instruction to save. 
    - maybe save CPSR

use the above to make fast.

for next week:
    - gpio interrupt for UART so that it speeds up
    - gpio interrupt for getc?
    - p1011?
    - system call: you can do a simple version.  is cute.

homeworks:
    - do getc/putc
    - replay
    - thread with save on stack with single instruction.
            - we need this b/c otherwise it's annoying to do pre-emptive.

    do same day is a +.

write up assembly.

have to write this up tonight.

------> fix sciatica <----

write simple save to all.

- how can you add general purpose monitoring?   give a function and have it attached
at any location?  or attach 

should have them do the queue / device four different ways?
    - interrupt handler
    - jump to scheduler.
    - enqueue event.

what do you do for concurrency?

would be cool to have them bootload over using FUSE
    - need a trivial virtual memory
    - need fuse
    - neet network bootloader (simple)

    just statically allocate everything.  super easy.  then they can redo this.
        - next level is to allow them to do threads themselves.


cs240L: 
    - can do threads / interrupts etc as the first couple weeks.  all the weird
    tradeoffs.

    do a bunch of devices.

    do the vm remote thing.
    do the laptop

    sipmle symex both for match v and for bootloader.

    have them make a fake checking system for the pi.  niceee.

        can you extend it to do threading and etc?

    oh: thread checking: iterate through and put a break everywhere.
        statistical discovery.
        run gprof.  get all the different code locations.
        then instrument just these.

        in general if you run it is code.  we assume no self modifying.  

        this is very nice.  very very nice actually.  can do this for 140e?

        good to have an sd or something.
            could make it so you transition each time.

        log the result.
            write out the check.

    this cs240L is interesting --- 
        alot of data sheets.
        1hr of discussion on monday, then lab

esp and bootloader: 
    can do an at, and see if it responds.  if not do bootloader.

they need a int n = gets_block(char *buf, size of buf, 40us);

can do 240L as a start for the devices course?
    digital
    time
    analogue
    i2c
    the second uart on the pi.
    esp8266
    
    do with interrupts, do with queue, with monitors.  priorities.  locks.
    
    the intereting thing: how do you tell that the thing works?
        i think the put/get gives a way to tell?   
            they are isolated: tag w/ thread id and the operation number.

    have to do variant,  which is useful for them to do anyway.
        e.g., uart you should see the same put/get for the fifo, but can add / remove
        interrupts.

        can then log uart_putc/getc as well.

    not sure how hard is to do the sd card?

    OH.  do a distributed OS / not system where you can control many of them and 
    you verify that the thing cannot lock up at any point no mattter what 
    values get sent.

    ah: can rerun the code with log checking as it runs?  

    this is very cool

email grant, laura, zain, bill.  ?

different locking approaches.

could use r/pi 3? 
    or risk v?  that'd be interesting.

    how quick can you get up and running?

oh, interesting.  b/c it is a small system and deterministic, can actually
hash the address space and stop when it matches.


sampling: gives the set of instructions.

they can do aggressive interleave and check that the puts and gets match up.

for cs240l
    osccilliscope, fast
    threads, fast
            equiv

    write the os in rust
    write for the risc v

    do as many devices as can in the first week.
    then make them distributed. [can do that now?]
        also have a broadcast

    more checking, fake. (do for cs49n?)

prelab: print out the bits.
    have to pull the cpsr in.

have to walk through the changes.
    - if you are resuming from a voluntary cswitch, can do less.

hvae them do fast threads for hw?  you need to mess around w/ it first.

have them do:
    syscall
    write in the exception vector themselves.

    change the saving.
    change to preload the stack.
    change how to resume?

  kicking the tires.

or: 
    change uart to give tx interrupt.  that would be more useful.    
    add a circular queue.

https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html

    8110:   e1200070    bkpt    0x0000
    8114:   e1200071    bkpt    0x0001
    8118:   e1200072    bkpt    0x0002
    811c:   e1200074    bkpt    0x0004
    8120:   e1200078    bkpt    0x0008
    8124:   e1200170    bkpt    0x0010
    8128:   e1200270    bkpt    0x0020
    812c:   e1200470    bkpt    0x0040
    8130:   e1200870    bkpt    0x0080

you have to jump back to a different place.

resume_point:
    0x00
resume:
    nop  # smash this with the instruction.
    ldr pc, [resume_pt]
