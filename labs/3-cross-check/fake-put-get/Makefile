MFLAGS =  --no-print-directory 

all: test-gpio test-put-get  simple-gpio-test   

test-put-get:
	make $(MFLAGS) -f template.mk NAME=test-put-get 

simple-gpio-test: 
	make $(MFLAGS) -f template.mk NAME=simple-gpio-test

test-gpio: 
	make $(MFLAGS) -f template.mk NAME=test-gpio

clean:
	make $(MFLAGS) -f template.mk clean
	rm -f test-gpio test-put-get  simple-gpio-test   
	rm -f test-gpio.out test-put-get.out  simple-gpio-test.out

.PHONY: all test-put-get simple-gpio-test test-gpio
