MAKEFLAGS +=  --no-print-directory

NAME= cmd-watch
CC = gcc
CFLAGS = -Wall -Werror -g  -Og -Wno-unused-function -Wno-unused-variable

ifndef CS140E_PATH
$(error CS140E_PATH not set!)
endif

LPI = $(CS140E_PATH)/libpi
# where lib unix is
LUNIX = $(CS140E_PATH)/labs/2-bootloader/unix-side/libunix/
LIB = $(LUNIX)/libunix.a

CFLAGS += -I$(LUNIX)

# uncomment the tags part of all to get tags.
all : libunix $(NAME) # tags

libunix:
	make -C $(LUNIX)

$(NAME): $(NAME).c $(LIB) ./Makefile
	$(CC) $(CFLAGS) $< -o $@ $(LIB)

tags:
	ctags *.[ch]

clean :
	rm -f *.o *~ Makefile.bak $(NAME) tags
	@make -s -C $(LUNIX) clean

.PHONY: libunix tags clean
