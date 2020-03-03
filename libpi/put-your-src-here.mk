# if you write a new file "foo.c" or "foo.S",  add it here as a "foo.o" 
# (the makefile will figure out how to produce it).
#
# if you add gpio.o or uart.o remove it from EXTRA_OBJS
YOUR_OBJS = uart.o gpio.o kmalloc.o sw-uart.o

# these are initial .o's we give you: to use your own, remove them
# and add yours
EXTRA_OBJS= cs140e-objs/uart-int.o 
