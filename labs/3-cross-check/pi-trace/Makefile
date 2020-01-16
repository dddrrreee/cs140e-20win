NAME := hello
TNAME = $(NAME).trace
# add the other functions you want to trace to this line.
TRACE_FNS = GET32 #  PUT32 get32 put32
# turns them into the right linker flag arguments.
TRACE_FLAGS := $(foreach o, $(TRACE_FNS), --wrap=$o)

# check that your path is defined.
ifndef CS140E_PATH
$(error CS140E_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

include $(CS140E_PATH)/libpi/includes.mk

# if you get source from other directories, add them to vpath too, ':' seperated
VPATH := .:
BUILD_DIR := ./objs


# if you want to compile everything in a directory, do something like this.
#   SOURCES := $(wildcard ./*.c)
#   OBJS := $(patsubst %.c, %.o, $(SOURCES))
OBJS := $(NAME).o 
# this is putting the objects in the BUILD_DIR
OBJS := $(foreach o, $(OBJS), $(BUILD_DIR)/$(notdir $o))

TRACE_OBJS=trace.o
TRACE_OBJS := $(foreach o, $(TRACE_OBJS), $(BUILD_DIR)/$(notdir $o))

# Get all depend files by replacing extensions
DEPS := $(OBJS:.o=.d) $(TRACE_OBJS:.o=.d)

all: libpi $(TNAME).bin # $(NAME).bin

# is there a better way to do this?
libpi:
	make -C $(LPP)

$(NAME).bin: $(MEMMAP) $(OBJS)
	$(LD) $(OBJS) -T $(LPP)/memmap -o $(NAME).elf  $(LPI)
	$(OD) -D $(NAME).elf > $(NAME).list
	$(OCP) $(NAME).elf -O binary $(NAME).bin

$(TNAME).bin: $(MEMMAP) $(OBJS) $(TRACE_OBJS)
	$(LD) $(OBJS) $(TRACE_FLAGS) -T $(LPP)/memmap -o $(TNAME).elf  $(LPI) $(TRACE_OBJS) $(LPI)
	$(OD) -D $(TNAME).elf > $(TNAME).list
	$(OCP) $(TNAME).elf -O binary $(TNAME).bin

run:
	my-install $(NAME).bin

tags: 
	ctags *.[chSs] */*.[chSs]

clean:
	rm -rf $(BUILD_DIR) $(NAME) *~ tags *.o *.bin *.elf *.list *.img Makefile.bak 

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

