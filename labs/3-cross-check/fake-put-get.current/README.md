### start of a simple user space testing library

You now have a directory `cs140e-20win//libpi-fake` at the top level.   
You should move your `fake-put-get.c` from last lab to the `libpi-fake` directory.  

To test that your implementation still works run:

    make clean
    make USE_BAD_RANDOM=1

Which uses the bad random we had in the previous lab.  The output should be:

    ./test-put-get | cksum 
    2091088470 10304
    ./simple-gpio-test | cksum
    108713007 384
    ./test-gpio  | cksum
    3380538640 2335
    ./test-gpio 3  | cksum
    3203140722 6715
    ./test-gpio 4  | cksum
    409326025 25274

To test that your code works with the new random:

    make clean
    make

Output should be:

    ./test-put-get | cksum 
    2186576372 9174
    ./simple-gpio-test | cksum
    1988872002 384
    ./test-gpio  | cksum
    3441322806 2335
    ./test-gpio 3  | cksum
    2353837218 6715
    ./test-gpio 4  | cksum
    3210579411 24756


Also look at `./blink.c` and run it: `make blink`.   This gives an example
of how to run complete pi program on your laptop.
