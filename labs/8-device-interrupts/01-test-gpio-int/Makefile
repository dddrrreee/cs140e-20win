# Makefile for a single file.  Makes it really hard to break anything.
MAKEFLAGS +=  --no-print-directory

# we've had some make issues going cross windows/mac/linux, so we just do brute stupid
# to make things work: no fancy rules, just always recompile.
PROGS = 0-test

PROG_BIN := $(patsubst %, %.bin, $(PROGS))


# check that your path is defined.
ifndef CS140E_PATH
$(error CS140E_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

include $(CS140E_PATH)/libpi/includes.mk

# if you want to compile everything in a directory, do something like this.
#   SOURCES := $(wildcard ./*.c)
#   OBJS := $(patsubst %.c, %.o, $(SOURCES))
OBJS := gpio-int.o

# if you want to add all the .h's as dependencies:
#   DEPS := $(wildcard ./*.h) $(LPI)
DEPS := $(wildcard ./*.h) $(LPI) ./Makefile

all: libpi $(OBJS) $(PROG_BIN)

libpi: 
	@make -s -C $(LPP)

fake:
	@make -f ./Makefile.unix NAME=$(NAME).fake
	# to run: ./hello.fake

$(OBJS): $(DEPS)

$*
# <LPI> will get rebuilt if anything changes, so we don't need any other
# dependencies.
%.bin: %.o $(MEMMAP) $(OBJS) $(DEPS)
	$(LD) $(START) $< $(OBJS) -T $(LPP)/memmap -o $*.elf  $(LPI)
	$(OD) -D  -S $*.elf > $*.list
	$(OCP) $*.elf -O binary $*.bin
	# my-install ./$*.bin
	
run:
	my-install $(NAME).bin

tags: 
	ctags *.[chSs] */*.[chSs]

clean:
	rm -rf $(BUILD_DIR) $(NAME) *~ tags *.o *.bin *.elf *.list *.img Makefile.bak 
	# @make -f ./Makefile.unix NAME=$(NAME).fake clean

.PHONY: libpi all run tags clean  fake
