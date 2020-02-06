MFLAGS =  --no-print-directory 
# MAKEFLAGS += --warn-undefined-variables

PROGS = simple-gpio-test # full-gpio-test

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
GPIO = $(LPI)/gpio.c gpio-int.c

CC=gcc
CFLAGS = -Wall -g -Og -std=gnu99  -Werror -Wno-unused-function 
CFLAGS += -I$(LPI) -I$(LUNIX) -DRPI_UNIX -I$(FLPI) -I$(LPI)/cs140e-interrupt

# test lib
TLIB = $(FLPI)/libpi-test.a

# might need other libs too -lm -lutil, etc
LIBS= $(TLIB)

# make fake lib pi first, then progs, then run.
all: libfake $(PROGS) 

# don't worry about deps: always run make
libfake:
	make -C $(FLPI)

#################################################################
# we manually list out every target and run command to try to keep
# the Makefile understdandable and hard to screw up.
# 
# <TLIB> will get triggered if anything changes, so we don't need any
# other dependencies

simple-gpio-test: simple-gpio-test.c $(GPIO) $(TLIB)
	$(CC) $(CFLAGS) $< $(GPIO) -o $@ $(LIBS)
	# ./simple-gpio-test 
	./simple-gpio-test | cksum
	./simple-gpio-test | grep -v dev_barrier | cksum

full-gpio-test: full-gpio-test.c $(GPIO) $(TLIB)
	$(CC) $(CFLAGS) $< $(GPIO) -o $@ $(LIBS)
	# ./full-gpio-test 
	./full-gpio-test | cksum
	./full-gpio-test | grep -v dev_barrier | cksum

clean:
	rm -f *.o *~  *.bak $(PROGS) *.out 
	make -C $(FLPI) clean

.PHONY: all clean run libfake
