# good breakdown: http://cs107e.github.io/guides/make/
CFLAGS = -Wall -Og -Werror -std=gnu99
CC = gcc
LIBS = -lm -lutil

PROGS = select-ex demand-ex pipe-ex signal-ex pipe-read-exit-ex wait-ex local-socket is-fd-open socket-ex test-redirect dup2-redirect kill-ex pipe-detect-parent-died pty-ex
 
all : $(PROGS)

# this runs <all> without the %.c
%:  %.c
	$(CC) $(CFLAGS) $^  -o $@ $(LIBS)

clean :
	rm -f *.o *~ Makefile.bak $(PROGS)
