#                           !!!DO NOT MODIFY THIS MAKEFILE!!!
#                           !!!DO NOT MODIFY THIS MAKEFILE!!!
#                           !!!DO NOT MODIFY THIS MAKEFILE!!!
#                           !!!DO NOT MODIFY THIS MAKEFILE!!!
#                           !!!DO NOT MODIFY THIS MAKEFILE!!!
#
# We may make changes to it later, which would lead to 30+ conflicts.   So please don't
# modify it.

# check that your path is defined.
ifndef CS140E_PATH
$(error CS140E_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif
include includes.mk

# if you get source from other directories, add them to vpath too.
VPATH := .:libc/:src/:my-src:
START= cs140e-start

# grab everything in libc and src
SOURCES     := $(wildcard ./libc/*.c  ./src/*.c  ./my-src/*.c)
ASM_SOURCES := $(wildcard ./libc/*.S  ./src/*.S)
OUR_OBJS := $(patsubst %.c, %.o, $(SOURCES)) $(patsubst %.S, %.o, $(ASM_SOURCES))

# this is where your code is.
include put-your-src-here.mk

# make sure that people cannot skip errors.
CFLAGS += -Wno-unused-function -Werror

BUILD_DIR := ./objs
TARGET = libpi.a

OBJS := $(OUR_OBJS)  $(YOUR_OBJS)

OBJS := $(foreach o, $(OBJS), $(BUILD_DIR)/$(notdir $o)) $(EXTRA_OBJS)
# Get all depend files by replacing extensions
DEPS := $(OBJS:.o=.d)  $(BUILD_DIR)/$(START).d

all: $(TARGET) $(START).o 

# we bundle start.s with the library: just make different ones for each
# different start so that the library is internally consistent.
$(TARGET): $(OBJS) $(SUPPORT_OBJS) 
	arm-none-eabi-ar crf $(TARGET) $(OBJS) $(SUPPORT_OBJS)
	$(OD) -D $@ > $@.list

# kind of gross.  we want this .o at the top-level so people can link to it easily.
# so we need a duplicate rule.
$(START).o: $(BUILD_DIR)/$(START).d
	@mv $(BUILD_DIR)/$(START).o .

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

ctags: 
	ctags *.[chSs] */*.[chSs]

clean:
	rm -rf $(BUILD_DIR) $(TARGET) *~ $(TARGET).list tags cs140e-start.o 
	make   --no-print-directory -C libc clean
	make   --no-print-directory -C src clean

.PHONY: clean all ctags
