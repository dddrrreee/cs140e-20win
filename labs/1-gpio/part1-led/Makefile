# makefile based on scott mcpeak's grad school page :)
# 	http://scottmcpeak.com/autodepend/autodepend.html
NAME=blink

# start.o must be first!
OBJS = start.o $(NAME).o gpio.o

ARM = arm-none-eabi
CC = $(ARM)-gcc
LD  = $(ARM)-ld
AS  = $(ARM)-as
OD  = $(ARM)-objdump
OCP = $(ARM)-objcopy

CFLAGS = -O -Wall -nostdlib -nostartfiles -ffreestanding  -march=armv6 -std=gnu99 
ASFLAGS = --warn --fatal-warnings -mcpu=arm1176jzf-s -march=armv6zk

all: $(NAME).bin

# if this does not work: you need to add the cs49n/bin directory (holding my-install) 
# into your path.
run:
	my-install $(NAME).bin

$(NAME).bin: memmap $(OBJS) 
	$(LD) $(OBJS) -T memmap -o $(NAME).elf 
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
	rm -f replay *.o *.d *.bin *.elf *.list *.img Makefile.bak *~ tags
