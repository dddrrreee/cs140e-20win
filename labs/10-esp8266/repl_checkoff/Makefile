CC = gcc
LIBESP_PATH=../libesp
ESP_LIB = $(LIBESP_PATH)/libesp-unix.a

LIBUNIX_PATH := $(CS140E_PATH)/libunix
LIBUNIX := $(LIBUNIX_PATH)/lib-support.unix.a
CFLAGS := -Wall -Werror -g -Wno-unused-function -Wno-unused-variable -I. -I$(LIBUNIX_PATH) -g -DRPI_UNIX -I$(LIBUNIX_PATH)/.. -I$(LIBESP_PATH)

# just make all .o's depend on everything.  simple.
SOURCES := $(wildcard ./*.c)
HDRS := $(wildcard ./*.h)
ALL_OBJS := $(patsubst %.c, %.o, $(SOURCES))
DEPS := ./Makefile $(LIBUNIX) $(LIBUNIX_PATH)/libunix.h $(HDRS)

PROGS = client

all : libunix libesp $(PROGS)

$(ALL_OBJS): $(DEPS) 

# make libunix quietly.  remove the '@' and the '-s' if you want to see what is 
# going on.
libunix:
	@make -s -C $(LIBUNIX_PATH)

libesp:
	@make -s -C $(LIBESP_PATH)

#unix-server: unix-server.o $(LIBUNIX) $(ESP_LIB)
	#$(CC) $(CFLAGS) $@.o -o $@ $(ESP_LIB) $(LIBUNIX) -lm -lutil
	# ./unix-server

%: %.o $(LIBUNIX) $(ESP_LIB)
	$(CC) $(CFLAGS) $@.o -o $@ $(ESP_LIB) $(LIBUNIX) -lm -lutil


crun:
	./client # /dev/ttyUSB1
srun:
	./server # /dev/ttyUSB0

tags:
	ctags *.[ch] $(LIBUNIX_PATH)/*.[ch]

clean :
	rm -f *.o *.a *~ Makefile.bak $(PROGS)
	make -C $(LIBESP_PATH) clean

.PHONY: clean tags   libunix run
