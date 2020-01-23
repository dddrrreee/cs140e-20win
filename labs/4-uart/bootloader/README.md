Last, hard test: make sure your UART and the base libpi.a works with the bootloader.

Why:
    - Previous tests could have falsely passed since your bootloader  was using the
    built-in `uart_init`.

    - Also, your bootloader was using a weird stripped-down `libpi` with a bunch 
    of code not written by you.   So we convert to using your `libpi.a`.

How:
    1. Copy your bootloader.c to this directory.
    2. Update it to use the your matching libpi routines.  You'll have to rename
      `reboot` (as `rpi_reboot`), `uart_rx_has_data` (as `uart_has_data`),
      and rename your `putk` to something else.
    3. Compile and copy `bootloader.bin` to your SD card as `kernel.img` (as usual).

As a final migration test:
    1. put your `uart.c` in `libpi/uart.c` and modify `put-your-src-here.mk` to 
      not use any of our `.o's`.

    2. Run:
        make clean
        make use-libpi-uart

    3. Check that the `bootloader.bin` works by booting `my-install ../uart/hello.bin

Congratuations!!!   At this point everything tricky was written by you.  "That is
all there is."
