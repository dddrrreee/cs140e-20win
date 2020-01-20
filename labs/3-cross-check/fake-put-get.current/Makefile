MFLAGS =  --no-print-directory 
# MAKEFLAGS += --warn-undefined-variables

PROGS = test-put-get simple-gpio-test test-gpio

# fake-put-get.c  simple-gpio-test.c  test-gpio.c  test-put-get.c

# check that your path is defined.
ifndef CS140E_PATH
$(error CS140E_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

# where libpi is.
LPI = $(CS140E_PATH)/libpi
# where the fake lib pi is
FLPI = $(CS140E_PATH)/libpi-fake
# where libunix is.
LUNIX = $(CS140E_PATH)/labs/2-bootloader/unix-side/libunix/
GPIO = $(LPI)/gpio.c

CC=gcc
CFLAGS = -Wall -g -Og -std=gnu99  -Werror -Wno-unused-function 
CFLAGS += -I$(LPI) -I$(LUNIX) -DRPI_UNIX -I$(FLPI)

# test lib
TLIB = $(FLPI)/libpi-test.a

# if we're supposed to test with the bad random from lab 3.
export USE_BAD_RANDOM

# might need other libs too -lm -lutil, etc
LIBS= $(TLIB)

# make fake lib pi first, then progs, then run.
all: libfake $(PROGS) run

# don't worry about deps: always run make
libfake:
	make -C $(FLPI)

#################################################################
# we manually list out every target and run command to try to keep
# the Makefile understdandable and hard to screw up.
# 
# <TLIB> will get triggered if anything changes, so we don't need any
# other dependencies

blink: blink.c $(TLIB)
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)
	./blink

test-put-get: test-put-get.c $(TLIB)
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)

simple-gpio-test: simple-gpio-test.c $(GPIO) $(TLIB)
	$(CC) $(CFLAGS) $< $(GPIO) -o $@ $(LIBS)

test-gpio: test-gpio.c $(GPIO) $(TLIB)
	$(CC) $(CFLAGS) $< $(GPIO) -o $@ $(LIBS)

# can comment out anything you don't want to execute
run: $(PROGS) 
	./test-put-get | cksum 
	./simple-gpio-test | cksum
	./test-gpio  | cksum
	./test-gpio 3  | cksum
	./test-gpio 4  | cksum

clean:
	rm -f *.o *~  *.bak $(PROGS) *.out blink
	make -C $(FLPI) clean

.PHONY: all clean run libfake
