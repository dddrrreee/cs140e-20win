# fix the makefile.

# to make put-get test:
#	make mktest
# to then check put-get:
#	make test
#
# to make all gpio tests:
#	make mk-gpio-tests
# to then check all the gpio tests
#	make gpio-tests
#
# makefile based on scott mcpeak's grad school page :)
# 	http://scottmcpeak.com/autodepend/autodepend.html

MK = make  --no-print-directory
CC = gcc

LPI = $(CS140E_PATH)/libpi 
LUNIX = ../../2-bootloader/unix-side/libunix/

CFLAGS = -I../ -I$(LPI) -I$(LUNIX) -Wall -g -Og -std=gnu99  -Werror  -Wno-unused-function -DRPI_UNIX

# uncomment test-gpio to make it.
all: test-put-get #  test-gpio

everything:
	@$(MK) test-put-get test-gpio
	@$(MK) mktest
	@$(MK) test
	@$(MK) mk-gpio-tests
	@$(MK) gpio-tests
	@echo "Everything passed!!!";

##############################################################################
# put-get tests: do these first.

test-put-get: test-put-get.o fake-put-get.o rpi-rand.o
	$(CC) $(CFLAGS) $^  -o $@ -lm


mktest:
	./test-put-get > test.put-get.out
	@$(MK) test

# simple test of fake memory.
test:
	@echo "---------------- running put-test test-----------------";
	./test-put-get  > out 
	diff out test.put-get.out
	cksum out
	@echo "----------------- Passed put-get test------------------";
	@echo "";

##############################################################################
# gpio-tests : do these second.


PART=0
mktest_gpio:
	./test-gpio $(PART) > test-gpio.$(PART).out
test_gpio:
	@echo "------------testing gpio PART=$(PART)----------------------"; 
	./test-gpio $(PART) > $(PART).out
	diff $(PART).out test-gpio.$(PART).out
	cksum $(PART).out
	@echo "------------Passed gpio test PART=$(PART)-------------------\n";

# remove comments to test.
mk-gpio-tests:
	@$(MK) PART=0 mktest_gpio
	@$(MK) PART=1 mktest_gpio
	@$(MK) PART=2 mktest_gpio
	@$(MK) PART=3 mktest_gpio

gpio-tests:
	@$(MK) PART=0 test_gpio
	@$(MK) PART=1 test_gpio
	@$(MK) PART=2 test_gpio
	@$(MK) PART=3 test_gpio
	@echo "Passed all gpio tests!";

test-gpio: test-gpio.o fake-put-get.o $(CS140E_PATH)/libpi/gpio.o
	$(CC) $(CFLAGS) $^  -o $@ -lm

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
	# $(CC) -MM $(CFLAGS) $*.c > $(notdir $*).d

	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

ctags:
	ctags *.[ch]
# remove compilation products
clean:
	rm -f $(NAME) *.o *.d Makefile.bak *~ *.bin tags out [0-3].out test-gpio test-put-get

cleanall:
	make clean
	rm -f *.out
