# makefile based on scott mcpeak's grad school page :)
# 	http://scottmcpeak.com/autodepend/autodepend.html

NAME= driver
OBJS := $(NAME).o external-code/bzt-sd.o fat32-helpers.o

# check that your path is defined.
LIBPI_PATH = $(CS140E_PATH)/libpi

# useful set of defines.
include $(LIBPI_PATH)/includes.mk


# if you want to use cronin.
# OBJS += external-code/emmc.o external-code/platform.o 
# LPI += ./libgcc.a
# CFLAGS += -DCRONIN

CPP_ASFLAGS =  -nostdlib -nostartfiles -ffreestanding   -Wa,--warn -Wa,--fatal-warnings -Wa,-mcpu=arm1176jzf-s -Wa,-march=armv6zk

CFLAGS += -Wstack-usage=512 -Iexternal-code

# -marm_prefer_ldrd_strd

CFLAGS += -Wno-unused-function -Werror

all: libpi $(NAME).bin run

run:
	my-install $(NAME).bin

# is there a better way to do this?
libpi:
	@make -C $(LIBPI_PATH)

$(NAME).bin: $(LIBPI_PATH)/memmap $(OBJS) $(LIBPI_PATH)/cs140e-start.o 
	$(LD) $(LIBPI_PATH)/cs140e-start.o $(OBJS) -T $(LIBPI_PATH)/memmap -o $(NAME).elf  $(LPI)
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
	rm -f replay *.o *.d *.bin *.elf *.list *.img Makefile.bak *~ external-code/*.o external-code/*.d
