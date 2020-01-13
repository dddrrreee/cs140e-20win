CFLAGS = -Wall -Werror -g  -Og -Ilibunix  -Wno-unused-function  -Wall
CC = gcc
SRC = my-install.c simple-boot.c  send-recv.c
OBJS = $(SRC:.c=.o)

all : libunix.a my-install run # tags 

libunix.a:
	make -C libunix

tags:
	ctags *.[ch]

run:
	./my-install ./hello.bin

my-install:  $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) -o my-install -lm libunix/libunix.a

clean :
	rm -f *.o *~ Makefile.bak my-install  tags
	make -C libunix clean

