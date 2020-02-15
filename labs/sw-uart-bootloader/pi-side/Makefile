CC= arm-none-eabi-gcc

LIBPI = libpi.small
CFLAGS = -I$(LIBPI) -I. -Wall  -Wno-unused-function -Og -nostdlib -nostartfiles -ffreestanding 

SRC = bootloader.c 
OBJS = $(SRC:.c=.o)

# Uncomment copy and fix up where your SD card is.
all : libpi kernel.img # copy

include $(CS140E_PATH)/libpi/includes.mk

libpi:
	make -C $(LPP)

start.o : start.s
	arm-none-eabi-as start.s -o start.o

kernel.img : memmap start.o $(OBJS)  $(LPI)
	arm-none-eabi-ld start.o $(OBJS) $(LPI) -T memmap -o bootloader.elf
	arm-none-eabi-objdump -D bootloader.elf > bootloader.list
	arm-none-eabi-objcopy bootloader.elf -O binary kernel.img

# change this to copy whereever your sd is mounted.
copy:
	cp kernel.img /media/engler/0403-0201/
	sync

clean :
	rm -f *.o *.bin *.elf *.list *.img *~ Makefile.bak
	make -C libpi.small clean

depend:
	makedepend -I$(LIBPI) *.[ch] 

.PHONY: clean depend copy libpi

# DO NOT DELETE

bootloader.o: ../shared-code/simple-boot.h libpi.small/rpi.h
