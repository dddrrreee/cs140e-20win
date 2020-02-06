# Makefile for a single file.  Makes it really hard to break anything.
MAKEFLAGS +=  --no-print-directory

# we've had some make issues going cross windows/mac/linux, so we just do brute stupid
# to make things work: no fancy rules, just always recompile.
#NAME := 0-circular-example
#NAME := 1-gpio-int-ex
NAME := 2-logic-analyzer

# check that your path is defined.
ifndef CS140E_PATH
$(error CS140E_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

include $(CS140E_PATH)/libpi/includes.mk

CFLAGS += -Wno-unused-function -Wno-unused-variable 

# if you want to compile everything in a directory, do something like this.
#   SOURCES := $(wildcard ./*.c)
#   OBJS := $(patsubst %.c, %.o, $(SOURCES))
OBJS := $(NAME).o 

# add all the .h's as dependencies, libpi and the makefile.
DEPS := $(wildcard ./*.h) $(LPI) ./Makefile

# uncomment the different tests.
all: libpi $(OBJS) $(NAME).bin

libpi: 
	make -C $(LPP)

$(OBJS): $(DEPS)

%.o: %.S
	$(CC) -c $(CPP_ASFLAGS) $< -o $@

# <LPI> will get rebuilt if anything changes, so we don't need any other
# dependencies.
%.bin: %.o $(MEMMAP) $(OBJS) $(DEPS)
	$(LD) $(START) $(OBJS) -T $(LPP)/memmap -o $*.elf  $(LPI)
	$(OD) -D   $*.elf > $*.list
	$(OCP) $*.elf -O binary $*.bin
	my-install ./$*.bin

run:
	my-install $(NAME).bin

tags: 
	ctags *.[chSs] $(LPP)/*.[ch] $(LPP)/*.[chSs]

clean:
	rm -rf $(BUILD_DIR) $(NAME) *~ tags *.o *.bin *.elf *.list *.img Makefile.bak  *.d 0-circular-example 1-gpio-int-ex 2-sw-uart-analyzer

cleanall:
	make -C $(LPP) clean
	make clean

.PHONY: libpi all run tags clean  cleanall
