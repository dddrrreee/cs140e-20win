CC = gcc
PROGS = assert-fail exit1 crash
CFLAGS = -Wall -Werror -g -Wno-unused-function -Wno-unused-variable

all : $(PROGS)

assert-fail: assert-fail.c
	$(CC) $(CFLAGS) $@.c -o $@

crash: crash.c
	$(CC) $(CFLAGS) crash.c -o crash

exit1: exit1.c
	$(CC) $(CFLAGS) exit1.c -o exit1

clean :
	rm -f *.o *~ Makefile.bak assert-fail exit1 crash

.PHONY: clean all
