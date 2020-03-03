# makefile based on scott mcpeak's grad school page :)
# 	http://scottmcpeak.com/autodepend/autodepend.html
NAME=vm
OBJS = $(NAME).o  your-vm-asm.o vm-helpers.o our-vm-asm.o our-vm.o
EXTRA_OBJS = 

LIBPI_PATH = $(CS140E_PATH)/libpi
# useful set of defines.
include $(LIBPI_PATH)/includes.mk

# if you want to use cronin.
# OBJS += external-code/emmc.o external-code/platform.o 
# LPI += ./libgcc.a
# CFLAGS += -DCRONIN

# CPP_ASFLAGS =  -nostdlib -nostartfiles -ffreestanding   -Wa,--warn -Wa,--fatal-warnings -Wa,-mcpu=arm1176jzf-s -Wa,-march=armv6zk

CFLAGS += -Wstack-usage=512 -I$(LIBPI_PATH)/libc

# -marm_prefer_ldrd_strd

CFLAGS += -Wno-unused-function 

all: libpi $(NAME).bin run # test # run

# wth.  make didn't used to cause problems like this.
test: $(NAME).bin
	my-install $(NAME).bin  > /tmp/out 2>&1 && grep HASH: /tmp/out > out
	diff out out.ref

mktest:
	my-install $(NAME).bin  > /tmp/out 2>&1 && grep HASH: /tmp/out > out.ref
	make test

run:
	my-install $(NAME).bin

# is there a better way to do this?
libpi:
	@make -C $(LIBPI_PATH)

$(NAME).bin: $(LIBPI_PATH)/memmap $(OBJS) $(LIBPI_PATH)/cs140e-start.o  $(EXTRA_OBJS)
	$(LD) $(LIBPI_PATH)/cs140e-start.o $(OBJS) $(EXTRA_OBJS) -T $(LIBPI_PATH)/memmap -o $(NAME).elf  $(LPI)
	$(OD) -D $(NAME).elf > $(NAME).list
	$(OCP) $(NAME).elf -O binary $(NAME).bin

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

# compile and generate dependency info;
# more complicated dependency computation, so all prereqs listed
# will also become command-less, prereq-less targets
#   sed:    strip the target (everything before colon)
#   sed:    remove any continuation backslashes
#   fmt -1: list words one per line
#   sed:    strip leading spaces
#   sed:    add trailing colons
%.o: %.c
	$(CC) -c $(CFLAGS) $*.c -o $*.o
	$(CC) -MM $(CFLAGS) $*.c > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

.s.o: 
	$(AS) $(ASFLAGS) $< -o $@

.S.o: 
	$(CC) -c $(CPP_ASFLAGS) $< -o $@

# remove compilation products
clean:
	rm -f replay  *.d *.bin *.elf *.list *.img Makefile.bak *~ $(NAME).o your-vm-asm.o vm-helpers.o
