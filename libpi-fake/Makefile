TARGET = libpi-test.a

# add any other object files here.
OBJS = fake-put-get.o pi-test.o pi-random.o fake-pi.o fake-gpio.o driver.o

# check that your path is defined.
ifndef CS140E_PATH
$(error CS140E_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

# where libpi is.
LPI = $(CS140E_PATH)/libpi
# where libunix is (currently)
LUNIX = $(CS140E_PATH)/labs/2-bootloader/unix-side/libunix/

CC=gcc
CFLAGS = -Wall -g -Og -std=gnu99  -Werror -Wno-unused-function 
CFLAGS += -I$(LPI) -I$(LUNIX) -DRPI_UNIX

# make USE_BAD_RANDOM=1  will make sure that you get the same results as the original
# lab 3.
ifdef USE_BAD_RANDOM
OBJS += rpi-rand.o
CFLAGS += -DUSE_BAD_RANDOM
endif

# overdepend: all .o's depend on all .h's  --- make deps as simple as possible.
DEPS := $(wildcard ./*.h) $(LPI)/rpi.h Makefile

all: $(TARGET)

$(OBJS): $(DEPS) 

$(TARGET): $(OBJS) $(DEPS) 
	ar cr $(TARGET) $(OBJS) 

clean:
	rm -f *.o $(TARGET) *~  *.bak  *.out

.PHONY: all clean
