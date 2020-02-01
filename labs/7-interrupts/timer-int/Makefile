# change this name to whatever your program is called
NAME=timer
# add any additional .o's here
OBJS = $(NAME).o interrupts-asm.o interrupts-c.o cstart.o  timer-interrupt.o

CFLAGS += -O2
# check that your path is defined.
ifndef CS49N_LIBPI_PATH
$(error CS49N_LIBPI_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

START = $(CS49N_LIBPI_PATH)/cs49n-start.o
MEMMAP = $(CS49N_LIBPI_PATH)/memmap

LPI_FAKE_PATH = $(CS49N_LIBPI_PATH)/../libpi-fake
LPI_FAKE = $(CS49N_LIBPI_PATH)/../libpi-fake/libpi-fake.a

# useful set of defines.
include $(CS49N_LIBPI_PATH)/includes.mk

CFLAGS += -Wstack-usage=512 -Wno-unused-function -Wno-unused-variable
CFLAGS += -Werror -O10

all: libpi $(NAME).bin  # run

# is there a better way to do this?
libpi:
	@make -C $(CS49N_LIBPI_PATH)

.PHONY: fake libpi clean

run: 
	pi-install $(NAME).bin

fake: 
	@make -C $(LPI_FAKE_PATH)
	gcc -I$(CS49N_LIBPI_PATH) -O -Wall -Werror $(NAME).c -o fake-$(NAME) $(LPI_FAKE)

ir-pair: ir-send.o ir-recv.o
	make NAME=ir-send
	make NAME=ir-recv

$(NAME).bin: $(START) $(MEMMAP) $(OBJS)
	$(LD) $(START) $(OBJS) -T $(MEMMAP) -o $(NAME).elf  $(LPI)
	$(OD) -D $(NAME).elf > $(NAME).list
	$(OCP) $(NAME).elf -O binary $(NAME).bin
	# ctags $(NAME).c


# remove compilation products
clean:
	rm -f fake-$(NAME) *.o *.d *.bin *.elf *.list *.img Makefile.bak *~ tags
	# make -C $(LPI_FAKE_PATH) clean


.PHONY: run libpi tags clean test

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

# should re-do this to use the libpi makefile.

%.d: %.c Makefile
	$(CC) $(CFLAGS) -c -MT $@ -MMD -MP -MF $@ $< -o $*.o

%.o : %.c
%.o : %.c %.d
	$(COMPILE.c) $(OUTPUT_OPTION) $<

.s.o: Makefile
	$(AS) $(ASFLAGS) $< -o $@

.S.o: Makefile
	$(CC) -c $(CPP_ASFLAGS) $< -o $@

tags:
	@rm -f tags
	ctags *.[chsS] $(CS49N_LIBPI_PATH)/*.[chsS]

test: fake
	./fake-$(NAME) 1 2> /tmp/$(NAME)-1.out
	cksum /tmp/$(NAME)-1.out
	./fake-$(NAME) 10 2> /tmp/$(NAME)-10.out
	cksum /tmp/$(NAME)-10.out


