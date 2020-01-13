all:

build:
	make -C pi-side
	make -C unix-side

clean:
	rm -f *~ */*~ *.bak
	make -C pi-side clean
	make -C unix-side clean

.PHONY: clean build
