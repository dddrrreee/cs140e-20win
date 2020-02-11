SUBDIRS= Makefile.unix #  Makefile.pi

.PHONY: all check clean
all check clean: $(SUBDIRS)

all: TARGET=all
check: TARGET=check
clean: TARGET=clean

# No, you can't do TARGET=$@, or at least I don't know how to.

# recursive call to make
$(SUBDIRS): force
	$(MAKE)  -f $@ $(TARGET)

clean:
	rm -f *~ *.bak tags 

.PHONY: force
	force :;
