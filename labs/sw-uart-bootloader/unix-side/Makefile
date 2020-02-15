CFLAGS = -Wall -Werror -g  -Og -Ilibunix  -Wno-unused-function  -Wall
CC = gcc
SRC = my-install.c simple-boot.c  send-recv.c
OBJS = $(SRC:.c=.o)

# uncomment:
#   - the 'run' target to automatically run the hello.bin code.
#   - the 'tags' target to run tags on the code.  this is super
#     helpful.
all : libunix my-install # run # tags 

libunix:
	make -C libunix

my-install:  $(OBJS)  libunix/libunix.a
	$(CC) $(CFLAGS) $(OBJS) -o my-install -lm libunix/libunix.a

tags:
	ctags *.[ch]

run:
	./my-install ./hello.bin


clean :
	rm -f *.o *~ Makefile.bak my-install  tags
	make -C libunix clean

.PHONY: run clean tags libunix
