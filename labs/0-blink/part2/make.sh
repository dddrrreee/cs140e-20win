    arm-none-eabi-as blink-pin20.s -o blink-pin20.o
    arm-none-eabi-objcopy blink-pin20.o -O binary blink-pin20.bin
    rm blink-pin20.o
