MK = make  --no-print-directory
CFLAGS = -O10 -g
CC = arm-none-eabi-gcc 
#-Wall -O

all: obj
#  $(EX)-pointer.o

obj: $(EX)-pointer.o
    
mk:
	$(MK) EX=0
	$(MK) EX=1
	$(MK) EX=2
	$(MK) EX=3
	$(MK) EX=4
	$(MK) EX=5
	$(MK) EX=6
	$(MK) EX=7
	$(MK) EX=8
	$(MK) EX=9
	$(MK) EX=10

$(EX)-pointer.o: $(EX)-pointer.c
	echo "\n\n\n\n\n\n\n";
	echo "Recompiling $(EX)-pointer.c ------------------------\n";
	echo " arm-none-eabi-gcc $(CFLAGS) -c $(EX)-pointer.c";
	arm-none-eabi-gcc $(CFLAGS) -c $(EX)-pointer.c
	arm-none-eabi-objdump -d -S $(EX)-pointer.o > $(EX)-pointer.sym.dis
	arm-none-eabi-objdump -d  $(EX)-pointer.o > $(EX)-pointer.dis
	cat $(EX)-pointer.dis
	echo "---------------------------------------------------------------\n";
	echo "\n\n\n\n\n\n\n\n";



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

clean:
	rm -f  *.dis *.o *~ *.s *.d
