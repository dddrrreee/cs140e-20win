## Prelab: vm coherence and operations.

Prelab is pretty simple.
   1. Read (you should have already, so this should be a re-read).
   2. Refactor your threads code so it's in `libpi` (should be fast).

### 1. Reading

As mentioned in class, make sure you read chapter B2 of the armv6
document, focusing on pages B2-16 til the end of the chapter.  This was
handed out in class, but it is also in the docs directory:

   - `14-vm/docs/armv6.b2-memory.annot.pdf`

There isn't a lot of prose, but you need to think carefully about why and
what you have to do.  Make a list of rules.   We will spend some initial
class time discussion going over this, which won't be very useful if
you are unfamiliar with the details.

IMO, these are the most subtle of the rules on the arm (and on many
chips), so being able to reason about them means that everything else
is comparatively easy.  It also means you have a real grasp of how
actual reality works and looks like; most implementors have no idea.

### 2. refactor 6-threads to put your code in `libpi/my-src` (quick)

As a preparatory step towards making user-level processes, you should
also refactor your threads lab so that the thread code is in your
`libpi/my-src`.  This should take 5-10 minutes, hopefully.   

In order to minimize boring bugs, I'd suggest doing it as follows:

  0. Make sure it still compiles before doing anything:

            % cd 6-threads/threads.new
            % make clean
            % make

  1.  Make a clean copy so we don't break working code:

            % cd ..
            % cp -r threads.new threads.refactor

  2. Add all the tests to the `Makefile` by changing the `all` target.
      For me:

            all: libpi $(OBJS) 4-test-exit.bin 1-test-thread.bin \
                            2-test.bin 3-test-cswitch.bin

  3. Get the output before we touch anything.  I run `tcsh` so that looks something
      like:

            % cd 6-threads.refactor
            % make -s | & grep -v arm-none > out

      Where `make -s` tells `make` to be somewhat quiet and we strip out the arm
      compilation calls and put the result in output file `out`.
    
  4. Refactor: move the thread code to your `libpi/my-src` directory so it 
       automatically gets compiled and added to `libpi`:  

            # from 6-threads.refactor
            % mv rpi-thread.c Q.h thread-asm.S ../../../libpi/my-src

  5. Refactor: Change the `Makefile` to not have the thread `.o's`:

       Previous:

            OBJS := test-asm.o rpi-thread.o thread-asm.o circular.o  

       Becomes:

            OBJS := test-asm.o

  6. Test:

            % make clean
            % make 
            % make -s | & grep -v arm-none > out2
            % diff out2 out

  7. Great!

This is a general pattern: whenever you are going to do a change that should not
change external behavior, always:
    1. Run every test you can before, capturing the output to a file.
    2. Make the change.
    3. Rerun every test, capturing the output to a file.
    4. Diff the output.

This will catch many errors.  Best is to add this to your `Makefile`
targets so it is done always and any deviation automatically flagged.
If you have output that changes but is not essential, prefix it with a
string of some kind (e.g., `DEBUG:`) so you can `grep` it out.
