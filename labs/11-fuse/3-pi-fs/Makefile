# makefile based on scott mcpeak's grad school page :)
# 	http://scottmcpeak.com/autodepend/autodepend.html

NAME= pi-fs
WHERE=./fake-pi

FUSE_CMD = ./$(NAME) -f -d -s $(WHERE)

LIBUNIX_PATH := $(CS140E_PATH)/libunix
LIBUNIX := $(LIBUNIX_PATH)/lib-support.unix.a
CFLAGS := -Wall -g -Wno-unused-function -Wno-unused-variable -I. -I$(LIBUNIX_PATH) -Og -DRPI_UNIX -I$(LIBUNIX_PATH)/..

OBJS = $(NAME).o 

FUSE_CMD = ./$(NAME) -f -d -s $(WHERE)

CC = gcc
CFLAGS += -D_FILE_OFFSET_BITS=64  `pkg-config fuse --cflags --libs`

all: check_os $(NAME) 

check_os:
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LINUX_LIBS=-lfuse
    FUSE_UNMOUNT=fusermount -uz
else
    FUSE_UNMOUNT=umount
endif

run:
	cat $(WHERE)/hello
	sleep 1
	cat $(WHERE)/hello

mount: 
	mkdir -p $(WHERE)
	./$(NAME) -f -d -s $(WHERE)

unmount:
	$(FUSE_UNMOUNT) $(WHERE)

fuse.run:
	my-install pi-side/pi-vmm.bin -exec  $(FUSE_CMD)

$(NAME):  $(OBJS) 
	$(CC) $(CFLAGS) $^ `pkg-config fuse --cflags --libs`  -o $@ -lm  $(LIBUNIX)


test:
	@ echo "----------start------------------------------"
	echo 1 > $(WHERE)/echo.cmd
	@ echo "----------2---------------------------------"
	echo 2 >> $(WHERE)/echo.cmd
	@ echo "----------3---------------------------------"
	echo "hello this is our test" >> $(WHERE)/echo.cmd
	@ echo "----------4---------------------------------"
	cat $(WHERE)/console
	@ echo "----------5---------------------------------"
	cat $(WHERE)/echo.cmd
	ls -R $(WHERE)
	cat $(WHERE)/console
	cat ./hello-fixed.bin > $(WHERE)/run.cmd
	cat ./hello-fixed.bin > $(WHERE)/run.cmd
	cat ./hello-fixed.bin > $(WHERE)/run.cmd
	echo 1 > $(WHERE)/reboot.cmd

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

ctags:
	ctags *.[ch]
# remove compilation products
clean:
	rm -f $(NAME) *.o *.d Makefile.bak *~  tags 
