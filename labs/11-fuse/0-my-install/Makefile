LIBUNIX=$(CS140E_PATH)/libunix
LU = $(LIBUNIX)/lib-support.unix.a

CFLAGS = -Wall -Werror -g  -O -I$(LIBUNIX) -Wno-unused-function
CC = gcc
SRC = my-install.c simple-boot.c  send-recv.c

OBJS = $(SRC:.c=.o)

all : libunix my-install pi-echo tags # run # tags 

libunix:
	make -C $(LIBUNIX)

tags:
	ctags *.[ch]

run:
	./my-install ./hello.bin

my-install:  $(OBJS)   $(LU)
	$(CC) $(CFLAGS) $(OBJS) -o my-install -lm $(LU)

pi-echo: pi-echo.c $(LU)
	$(CC) $(CFLAGS) $< -o pi-echo -lm $(LU)

install:
	cp my-install pi-echo ~/bin/

clean :
	rm -f *.o *~ Makefile.bak my-install  tags pi-echo

.PHONY: libunix clean run tas
