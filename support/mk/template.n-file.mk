# compiles n files into single executable: grabs all .c files automatically.
# maybe make it check if OBJS is already defined.

# we've had some make issues going cross windows/mac/linux, so we just do brute stupid
# to make things work: no fancy rules, just always recompile.

# NAME = hello

ifndef NAME
$(error NAME of target not defined!)
endif

# check that your path is defined.
ifndef CS140E_PATH
$(error CS140E_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

include $(CS140E_PATH)/libpi/includes.mk

# if you want to compile everything in a directory, do something like this.
SOURCES := $(wildcard ./*.c)
OBJS := $(patsubst %.c, %.o, $(SOURCES))
# OBJS := $(NAME).o

# just say that everything depends on all the .h's.
DEPS := $(wildcard ./*.h) $(LPI)
DEPS := $(DEPS) $(LPI) ./Makefile 

all: libpi $(OBJS) $(NAME).bin

libpi: 
	@make -s -C $(LPP)

$(OBJS): $(DEPS)

# <LPI> will get rebuilt if anything changes, so we don't need any other
# dependencies.
$(NAME).bin: $(MEMMAP) $(OBJS) $(LPI) ./Makefile
	$(LD) $(OBJS) -T $(LPP)/memmap -o $(NAME).elf  $(LPI)
	@$(OD) -D $(NAME).elf > $(NAME).list
	$(OCP) $(NAME).elf -O binary $(NAME).bin

run:
	my-install $(NAME).bin

tags: 
	ctags *.[chSs] */*.[chSs]

clean:
	rm -rf $(BUILD_DIR) $(NAME) *~ tags *.o *.bin *.elf *.list *.img Makefile.bak 

.PHONY: libpi all run tags clean 
