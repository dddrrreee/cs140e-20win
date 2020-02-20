NAME = send-pi-prog

CC = gcc
LPP = $(CS140E_PATH)/libpi

LIBUNIX_PATH := $(CS140E_PATH)/libunix
LIBUNIX := $(LIBUNIX_PATH)/lib-support.unix.a
CFLAGS := -Wall -Werror -g -Wno-unused-function -Wno-unused-variable -I. -I$(LIBUNIX_PATH) -g -DRPI_UNIX -I$(LPP) -I../pi-side

# just make all .o's depend on everything.  simple.
SOURCES := $(wildcard ./*.c)
HDRS := $(wildcard ./*.h)
ALL_OBJS := $(patsubst %.c, %.o, $(SOURCES)) $(FAKE_OBJS)
DEPS := ./Makefile $(LIBUNIX) $(LIBUNIX_PATH)/libunix.h $(HDRS)

all : libunix hello-fixed $(NAME)

$(ALL_OBJS): $(DEPS) 

# make libunix quietly.  remove the '@' and the '-s' if you want to see what is 
# going on.
libunix:
	@make -s -C $(LIBUNIX_PATH)

hello-fixed:
	make -C ../../hello-fixed/

%: %.o $(LIBUNIX) 
	$(CC) $(CFLAGS) $@.o -o $@ $(LIBUNIX) 

run:
	my-install ../pi-side/get-code.bin -exec  ./$(NAME) < ../../hello-fixed/hello-fixed.bin
tags:
	ctags *.[ch] $(LIBUNIX_PATH)/*.[ch]

clean :
	rm -f *.o *.a *~ Makefile.bak $(NAME)

.PHONY: clean tags   libunix run hello-fixed
