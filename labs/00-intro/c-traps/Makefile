# how do you make it compile each one of them programmatically?

CFLAGS = 
#-Wall -O

all: doall


run:
	gcc $(CFLAGS) 1-largest.c -o 1-largest
	./1-largest

doall:
	@echo "--------------------------------------------------------\n"
	gcc $(CFLAGS) 1-largest.c -o 1-largest
	./1-largest

	@gcc $(CFLAGS) 2-sideeffects.c -o 2-sideeffects
	@gcc $(CFLAGS) 3-comma.c -o 3-comma
	@gcc $(CFLAGS) 4-and.c -o 4-and
	@gcc $(CFLAGS) 5-or.c -o 5-or
	@gcc $(CFLAGS) 6-or.c -o 6-or
	@gcc $(CFLAGS) 7-precedence.c -o 7-precedence

	@gcc $(CFLAGS) 8-eval.c -o 8-eval
	@gcc $(CFLAGS) 9-eval.c -o 9-eval
	@gcc $(CFLAGS) 10-control.c -o 10-control
	@gcc $(CFLAGS) 11-cpp.c -o 11-cpp


#	gcc $(CFLAGS) 12-assertnow.c -o 12-assertnow

clean:
	rm -f *~ *.o a.out 
	rm -f 1-largest 2-sideeffects 3-comma 4-and 5-or 6-or
	rm -f 0-main  8-eval 9-eval 10-control 7-precedence 11-cpp
