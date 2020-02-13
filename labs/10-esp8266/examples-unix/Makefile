# good breakdown: http://cs107e.github.io/guides/make/
CFLAGS = -Wall -Og -Werror -std=gnu99
CC = gcc
LIBS = -lm -lutil

PROGS = client server
 
all : $(PROGS)

# this runs <all> without the %.c
%:  %.c
	$(CC) $(CFLAGS) $^  -o $@ $(LIBS)

clean :
	rm -f *.o *~ Makefile.bak $(PROGS) a.out
