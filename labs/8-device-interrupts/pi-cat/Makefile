NAME= pi-cat

LIBUNIX=$(CS140E_PATH)/libunix
LU = $(LIBUNIX)/lib-support.unix.a

CFLAGS = -Wall -Werror -g  -O -I$(LIBUNIX)
CC = gcc
OBJS = $(NAME).o

all : libunix pi-cat #tags # run # tags 

libunix:
	make -C $(LIBUNIX)

tags:
	ctags *.[ch]

run:
	./pi-cat

$(NAME):  $(OBJS)   $(LU)
	$(CC) $(CFLAGS) $(OBJS) -o $@ -lm $(LU)

clean :
	rm -f *.o *~ Makefile.bak $(NAME) tags
	make -C $(LIBUNIX) clean

.PHONY: libunix clean run tas
