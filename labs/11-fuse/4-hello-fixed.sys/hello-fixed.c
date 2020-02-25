#include "rpi.h"
#include "pi-message.h"

#define put_byte uart_putc

// similar to how you sent printk's from your bootloader
// to the unix side, we send a message:
//     {PI_PRINT_CHAR, ch:uint8}
// to the unix side to print each character.  doing this 
// a char at a time (vs string) is pretty nuts; feel free
// to change!  we do like this to keep things simple.
static int net_putchar(int ch) {
    uint8_t c = ch;
    put_byte(PI_PRINT_CHAR);
    put_byte(c);
    return ch;
}

void notmain(void) {
    rpi_set_putc(net_putchar);
	printk("hello world from address %p\n", (void*)notmain);
	return;
}
