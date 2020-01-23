# Makefile for a single file.  Makes it really hard to break anything.
MAKEFLAGS +=  --no-print-directory

# we've had some make issues going cross windows/mac/linux, so we just do brute stupid
# to make things work: no fancy rules, just always recompile.
NAME = bootloader

# check that your path is defined.
ifndef CS140E_PATH
$(error CS140E_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

include $(CS140E_PATH)/libpi/includes.mk

# if you want to compile everything in a directory, do something like this.
#   SOURCES := $(wildcard ./*.c)
#   OBJS := $(patsubst %.c, %.o, $(SOURCES))
OBJS := $(NAME).o uart.o

NAME = bootloader
MEMMAP = ./memmap
START = ./start.o
BOOT = ./bootloader.o
UART = ../uart/uart.o

# order matters
OBJS = $(START) $(BOOT) $(UART)
CFLAGS += -I../../2-bootloader/pi-side

# if you want to add all the .h's as dependencies:
#   DEPS := $(wildcard ./*.h) $(LPI)
DEPS := $(LPI) ./Makefile  $(MEMAP)

all: support  $(NAME).bin #  $(OBJS) $(NAME).bin fake

$(OBJS): $(DEPS)

support: 
	@make -s -C $(LPP)
	@make -C ../uart

# <LPI> will get rebuilt if anything changes, so we don't need any other
# dependencies.
$(NAME).bin: $(DEPS) $(OBJS)
	$(LD) $(OBJS) -T $(MEMMAP) -o $(NAME).elf  $(LPI)
	@$(OD) -D $(NAME).elf > $(NAME).list
	$(OCP) $(NAME).elf -O binary $(NAME).bin

# don't use the ../uart.o
use-libpi-uart: $(DEPS) $(OBJS)
	$(LD) $(START) $(BOOT) -T $(MEMMAP) -o $(NAME).elf  $(LPI)
	@$(OD) -D $(NAME).elf > $(NAME).list
	$(OCP) $(NAME).elf -O binary $(NAME).bin


run:
	my-install $(NAME).bin

tags: 
	ctags *.[chSs] */*.[chSs]

clean:
	rm -rf $(BUILD_DIR) $(NAME) *~ tags *.o *.bin *.elf *.list *.img Makefile.bak 
	@make -f ./Makefile.unix NAME=$(NAME).fake clean

.PHONY: libpi all run tags clean  fake
