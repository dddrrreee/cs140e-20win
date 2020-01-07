CFLAGS = 
#-Wall -O

all:
	arm-none-eabi-gcc -O10 -c 0-timer.c
	arm-none-eabi-objdump -d 0-timer.o > 0-timer.dis

	arm-none-eabi-gcc -O10 -c 1-timer.c
	arm-none-eabi-objdump -d 1-timer.o > 1-timer.dis

	arm-none-eabi-gcc -O10 -c 1-timer-fix.c
	arm-none-eabi-objdump -d 1-timer-fix.o > 1-timer-fix.dis

	arm-none-eabi-gcc -O10 -c 2-wait.c
	arm-none-eabi-objdump -d 2-wait.o > 2-wait.dis
	# cat 2-wait.s

	arm-none-eabi-gcc -O10 -c 3-fb.c
	arm-none-eabi-objdump -d 3-fb.o > 3-fb.dis

	arm-none-eabi-gcc -O10 -c 4-fb.c
	arm-none-eabi-objdump -d 4-fb.o > 4-fb.dis

clean:
	rm -f  *.dis *.o *~ *.s
