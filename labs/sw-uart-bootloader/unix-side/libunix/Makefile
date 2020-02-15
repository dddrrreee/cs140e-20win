# WARNING:  the way this is setup, you cannot have files with the same 
#           name in different directories --- when the dependencies
#           are thrown into the BUILD_DIR they will collide.   to fix
#           this we'd have to move the directory structure in there,
#           or do some kind of name flatting.
#
#          you might want to leave the dependencies in the different
#          source directories, but this doesn't work if they are changed
#          by different build targets.  one approach might be to move
#          the makefile into the subdir rather than having it use
#          conditional compilation.
#
#  maybe this?
#      https://stackoverflow.com/questions/1139271/makefiles-with-source-files-in-different-directories

# OBJS = set-tty-8n1.o  tty.o open-fake-tty.o exec-client.o watchdog.o close-open-fds.o fork-exec.o tty-gone.o time-get-usec.o get-exitcode.o write-exact.o can-read.o is-pid-dead.o

# ../pi-cat/tty.o ../esp8622-pi/circular.o ./libsupport.o is-fd-open.o rd-wr-exact.o kid-died.o exit-code.o exec-server.o can-read.o catch-sigint.o async-read.o read-time.o is-eof.o run-system.o

# if you get source from other directories, add them to vpath too.
# e.g:
#   VPATH := .:cs140e-libc/:cs140e-src
VPATH := .

SOURCES := $(wildcard ./*.c)
OBJS := $(patsubst %.c, %.o, $(SOURCES))

# LIB_UNIX is defined for cases where we share the same code b/n 
# the pi and your UNIX laptop
CFLAGS = -Og -Wall -Wno-unused-variable -Wno-unused-function -Werror -DLIB_UNUX -I.
CC = gcc

TARGET := libunix.a
BUILD_DIR := ./objs

OBJS := $(foreach o, $(OBJS), $(BUILD_DIR)/$(notdir $o))
# Get all depend files by replacing extensions
DEPS := $(OBJS:.o=.d) 

all: $(TARGET)

# we bundle start.s with the library: just make different ones for each
# different start so that the library is internally consistent.
$(TARGET): $(OBJS) 
	ar cr $(TARGET) $(OBJS) 


# XXX: We seem to need this rule to stop compilation with an error if the 
# .c file has one: otherwise make does not stop compilation when generating 
# .d.  If compilation goes correctly, is not triggered.
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
	rm -rf $(BUILD_DIR) $(TARGET) *~ tags *.o

.PHONY: clean all ctags
