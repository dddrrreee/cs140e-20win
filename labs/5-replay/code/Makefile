CC = gcc
TRACE_OBJS = interpose.o log-util.o

PATH_LIB_SUPPORT = $(CS140E_PATH)/libunix
LIB_SUPPORT = $(PATH_LIB_SUPPORT)/lib-support.unix.a

# just make all .o's depend on everything.  simple.
SOURCES := $(wildcard ./*.c)
OBJS := $(patsubst %.c, %.o, $(SOURCES))
DEPS := Makefile $(LIB_SUPPORT) $(PATH_LIB_SUPPORT)/libunix.h interpose.h

CFLAGS = -Wall -Werror -g -Wno-unused-function -Wno-unused-variable -I.  -I$(PATH_LIB_SUPPORT)  -I$(CS140E_PATH)/labs/2-bootloader

all : support pretty-print replay # run.replay # trace run.trace 

$(OBJS): $(DEPS) 

support:
	make -C $(PATH_LIB_SUPPORT)
	make -C bad-unix-side

trace: $(TRACE_OBJS) trace.o $(LIB_SUPPORT)
	$(CC) $(CFLAGS) $@.o $(TRACE_OBJS) -o $@ $(LIB_SUPPORT) -lm -lutil

# trivial example of running a trace
run.trace:
	./trace my-install ./hello.bin
	wc ./log.interpose.txt
	cksum ./log.interpose.txt

replay: $(TRACE_OBJS) replay.o $(LIB_SUPPORT)
	$(CC) $(CFLAGS) $@.o $(TRACE_OBJS) -o $@ $(LIB_SUPPORT) -lm -lutil

# run replay of the trace above.
run.replay:
	./replay ./log.interpose.txt my-install ./hello.bin

# should work on the logs
pretty-print: Makefile $(LIB_SUPPORT) pretty-print.o $(PATH_LIB_SUPPORT)/libunix.h
	$(CC) $(CFLAGS) $@.o -o $@ $(LIB_SUPPORT) -lm -lutil
	# ./pretty-print ./log.interpose.txt

tags:
	ctags *.[ch] $(PATH_LIB_SUPPORT)/*.[ch]

clean :
	rm -f *.o *~ Makefile.bak $(TARGET) trace replay tags pretty-print
	make clean -C $(PATH_LIB_SUPPORT)

.PHONY: clean tags run.replay run.trace
