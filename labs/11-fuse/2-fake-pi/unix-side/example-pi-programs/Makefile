PROGS = 0-reboot  2-blink

include $(CS140E_PATH)/libpi/includes.mk
CFLAGS += -Wno-unused-function -Wno-unused-variable 

# if you want to compile everything in a directory, do something like this.
#   SOURCES := $(wildcard ./*.c)
#   OBJS := $(patsubst %.c, %.o, $(SOURCES))
OBJS := $(patsubst %, %.o, $(PROGS)) 
PROGS := $(patsubst %, %.bin, $(PROGS)) 

# add all the .h's as dependencies, libpi and the makefile.
DEPS := $(MEMMAP) $(LPI)

# uncomment the different tests.
all: libpi $(OBJS) $(PROGS) # $(NAME).bin # run

libpi: 
	@ # @make -s -C $(LPP)
	make -C $(LPP)

$(OBJS): $(DEPS)
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.S
	$(CC) -c $(CPP_ASFLAGS) $< -o $@

# rules to generate .elf->.list->.bin
%.elf: $(MEMMAP) $(OBJS) $(DEPS) 
	$(LD) $(START) $*.o -T $(LPP)/memmap -o $*.elf  $(LPI)
%.list: %.elf
	$(OD) -D   $*.elf > $*.list
%.bin: %.list
	$(OCP) $*.elf -O binary $*.bin

run:
	my-install ./$(NAME).bin

tags: 
	ctags *.[chSs] $(LPP)/*.[ch] $(LPP)/*.[chSs]

clean:
	rm -rf $(PROGS) *.fake *~ tags *.o *.bin *.elf *.list *.img Makefile.bak  *.d

.PHONY: libpi all tags clean run

# stop make from deleting these suffix'd files.
.PRECIOUS: %.list %.elf
