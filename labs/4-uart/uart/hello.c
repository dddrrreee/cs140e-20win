#include "rpi.h"

static void my_putk(char *s) {
    for(; *s; s++)
        uart_putc(*s);
}

void notmain(void) {
    uart_init();
    my_putk("hello world\n");
}
