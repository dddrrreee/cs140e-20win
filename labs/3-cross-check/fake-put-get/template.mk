NAME=test-put-get

# check that your path is defined.
ifndef CS140E_PATH
$(error CS140E_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif
LPI = $(CS140E_PATH)/libpi

CC=gcc

# if you get source from other directories, add them to vpath too, ':' seperated
# also have to add each extra-src directory.  ugh.
VPATH := .:$(LPI)/libc:$(LPI)
BUILD_DIR := ./objs

LUNIX = ../../2-bootloader/unix-side/libunix/
CFLAGS = -I../ -I$(LPI) -I$(LUNIX) -Wall -g -Og -std=gnu99  -Werror  -Wno-unused-function -DRPI_UNIX

EXTRA_SRC = rpi-rand.c gpio.c

# if you want to compile everything in a directory, do something like this.
SOURCES := $(wildcard ./*.c) $(EXTRA_SRC)
OBJS := $(patsubst %.c, %.o, $(SOURCES))
# OBJS := $(NAME).o

# this is putting the objects in the BUILD_DIR
OBJS := $(foreach o, $(OBJS), $(BUILD_DIR)/$(notdir $o))
# Get all depend files by replacing extensions
DEPS := $(OBJS:.o=.d)

all: $(NAME)  run

$(NAME): ./objs/$(NAME).o ./objs/fake-put-get.o ./objs/rpi-rand.o ./objs/gpio.o
	$(CC) $(CFLAGS) $^  -o $@ -lm

run:
	./$(NAME) > ./$(NAME).out
	cat ./$(NAME).out
	cksum ./$(NAME).out

tags: 
	ctags *.[chSs] */*.[chSs]

clean:
	rm -f *.o *.d Makefile.bak *~ *.bin tags out [0-3].out test-gpio test-put-get
	rm -rf $(BUILD_DIR)

.PHONY: clean all tags libpi  run

#################################################################################
# the rest of this is dependency nonsense.   must be a cleaner way?
#

# XXX: We seem to need this rule to stop compilation with an error if the .c file
# has one: otherwise make does not stop compilation when generating .d.
# If compilation goes correctly, is not triggered.
$(BUILD_DIR)/%.o : %.c
$(BUILD_DIR)/%.o : %.c $(BUILD_DIR)/%.d
	$(COMPILE.c) $(OUTPUT_OPTION) $<

# XXX: I think we need this to catch errors in .S as well.
$(BUILD_DIR)/%.o : %.S
$(BUILD_DIR)/%.o : %.S $(BUILD_DIR)/%.d
	$(CC) $(CPP_ASFLAGS) $<

# Rules to automatically generate dependencies and put in build directory
# We want to recompile everything if the makefile changes.
$(BUILD_DIR)/%.d: %.c Makefile
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -MT $@ -MMD -MP -MF $@ $< -o $(BUILD_DIR)/$(notdir $*).o

$(BUILD_DIR)/%.d: %.S Makefile
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CPP_ASFLAGS) -c -MT $@ -MMD -MP -MF $@ $< -o $(BUILD_DIR)/$(notdir $*).o

# -MF  write the generated dependency rule to a file
# -MG  assume missing headers will be generated and don't stop with an error
# -MM  generate dependency rule for prerequisite, skipping system headers
# -MP  add phony target for each header to prevent errors when header is missing
# -MT  add a target to the generated dependency

# I don't get why we need this.  check the mad-make guy.
#   http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
$(BUILD_DIR)/%.d: ;
.PRECIOUS: $(BUILD_DIR)/%.d

# *unbelievable*: make clean doesn't skip this include even though the 
# target is after.
#   https://www.gnu.org/software/make/manual/html_node/Goals.html
ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

