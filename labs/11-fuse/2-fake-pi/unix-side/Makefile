LPP = $(CS140E_PATH)/libpi
EX_DIR = ./example-pi-programs

##########################################
# organized sort of by ease.  should work
# through them one at a time.
#
# NAME  := $(EX_DIR)/0-reboot.c
# NAME  := $(EX_DIR)/1-put32.c
NAME  := $(EX_DIR)/2-blink.c
# NAME  := $(EX_DIR)/3-bug.c
# NAME  := $(EX_DIR)/4-bug.c
# NAME  := $(EX_DIR)/5-bug.c

## comment out gpio.c here to use your higher level code.
OUR_SRC := fake-pi.c $(LPP)/cs140e-private/gpio.c
OUR_OBJS := $(patsubst %.c, %.o, $(OUR_SRC)) 
OUR_OBJS := $(foreach o, $(OUR_OBJS), $(notdir $o))

# pull out the names etc.
OBJS :=  $(patsubst %.c, %.o, $(NAME))
FAKE_PROG := $(patsubst %.c, %.fake, $(NAME))

CC = gcc
LPP = $(CS140E_PATH)/libpi

LIBUNIX_PATH := $(CS140E_PATH)/libunix
LIBUNIX := $(LIBUNIX_PATH)/lib-support.unix.a
CFLAGS := -Wall -Werror -g -Wno-unused-function -Wno-unused-variable -I. -I$(LIBUNIX_PATH) -g -DRPI_UNIX -I$(LPP) -I../pi-side -Og

# just make all .o's depend on everything.  simple.
SOURCES := $(wildcard ./*.c)
HDRS := $(wildcard ./*.h)
ALL_OBJS := $(patsubst %.c, %.o, $(SOURCES)) $(FAKE_OBJS) $(OUR_OBJS)
DEPS := ./Makefile $(LIBUNIX) $(LIBUNIX_PATH)/libunix.h $(HDRS)

all : libs $(FAKE_PROG) run

libs:
	make -s -C $(LIBUNIX_PATH)
	make -C ../pi-side

$(ALL_OBJS): $(DEPS) 

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

%.fake: %.c $(OUR_OBJS) $(DEPS)
	$(CC) $(CFLAGS) $< $(OUR_SRC) -o $@ $(LIBUNIX) 

tags:
	ctags *.[ch] $(LIBUNIX_PATH)/*.[ch]
run:
	my-install ../pi-side/pi-vmm.bin -exec $(FAKE_PROG)

clean :
	rm -f *.o *.a *~ Makefile.bak $(FAKE_PROGS) $(EX_DIR)/*~ $(EX_DIR)/*.fake

.PHONY: clean tags libs run
