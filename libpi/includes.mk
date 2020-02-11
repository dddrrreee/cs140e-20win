# setup useful variables that can be used by make.

ARM = arm-none-eabi
CC = $(ARM)-gcc
LD  = $(ARM)-ld
AS  = $(ARM)-as
AR = $(ARM)-ar
OD  = $(ARM)-objdump
OCP = $(ARM)-objcopy
CS140E_LIBPI_PATH = $(CS140E_PATH)/libpi
LPP = $(CS140E_LIBPI_PATH)
LPI = $(CS140E_LIBPI_PATH)/libpi.a
START = $(LPP)/cs140e-start.o

MEMMAP=$(LPP)/memmap

ifndef CS140E_LIBPI_PATH
$(error CS140E_LIBPI_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

# CFLAGS = -O -Wall -nostdlib -nostartfiles -ffreestanding  -march=armv6 -std=gnu99 -I$(CS140E_LIBPI_PATH) -I.

INC = -I$(CS140E_LIBPI_PATH) -I.  -I$(CS140E_LIBPI_PATH)/cs140e-src -I$(CS140E_LIBPI_PATH)/cs140e-interrupt

CFLAGS = -Og -Wall -nostdlib -nostartfiles -ffreestanding -mcpu=arm1176jzf-s -mtune=arm1176jzf-s  -std=gnu99 $(INC) -ggdb -Wno-pointer-sign

ASFLAGS = --warn --fatal-warnings  -mcpu=arm1176jzf-s -march=armv6zk

CPP_ASFLAGS =  -nostdlib -nostartfiles -ffreestanding   -Wa,--warn -Wa,--fatal-warnings -Wa,-mcpu=arm1176jzf-s -Wa,-march=armv6zk   $(INC)
