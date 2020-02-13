SUBDIRS= Makefile.unix Makefile.pi

.PHONY: all check clean tags
all check clean tags: $(SUBDIRS)

all: TARGET=all
check: TARGET=check
clean: TARGET=clean
tags: TARGET=tags

# No, you can't do TARGET=$@, or at least I don't know how to.

# recursive call to make
$(SUBDIRS): force
	$(MAKE)  -f $@ $(TARGET)

clean:
	rm -f *~ *.bak tags

.PHONY: force
	force :;
