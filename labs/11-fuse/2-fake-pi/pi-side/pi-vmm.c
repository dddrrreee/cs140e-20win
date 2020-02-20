/*
 * engler, cs140e: trivial "virtual machine" driver for the pi-side that
 * recieves messages from your laptop over the uart (you could change to 
 * use esp8266 or anything else) and acts on them.
 *
 * our use today: run a pi program on your laptop but ship the pi-specific
 * parts (turning pins on/off, rebooting) to the pi itself.  this is just 
 * a proof of concept.
 * 
 * note: difficult to do this without interrupts since the unix side can easily
 * overrun the pi's 8 byte fifo.   use ours if yours is flakey.
 *
 * don't worry: most of the file is comments!
 */

#include "rpi.h"
#include "timer-interrupt.h"
#include "uart-int.h"
#include "pi-message.h"

void interrupt_vector(unsigned pc) {
    uart_interrupt_handler();
}

// can change these to use other methods (e.g., esp).
// could make them function pointers, but this is easy.
#define get_byte uart_getc_int
#define put_byte uart_putc_int

static void read_exact(void *data, unsigned n) {
    uint8_t *p = data;
    for(int i = 0; i < n; i++)
        p[i] = get_byte();
}
static void write_exact(const void *data, unsigned n) {
    const uint8_t *p = data;
    for(int i = 0; i < n; i++)
        put_byte(p[i]);
}
static unsigned get_uint32(void) {
    unsigned u;
    read_exact(&u,4);
    return u;
}
static void put_uint32(unsigned u) {
    write_exact(&u,4);
}


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
    int_init();
    uart_init_with_interrupts();
    system_enable_interrupts();

    // we override printk to use our network putchar
    rpi_set_putc(net_putchar);

    // we can have race conditions in all sorts of ways.  in this
    // case, if the unix side starts sending bytes before we 
    // are booted up, we would lose them.  conversely, if
    // we signaled we were ready, then after a reboot, before you
    // ran my-install the unix side would miss it.  
    if(get_byte() != PI_READY)
        panic("what is going on?\n");
    put_byte(PI_READY);

    // you should see this on your unix side.
    printk("PI VMM: about to start pi vmm\n");

    // loop forever, processing each method.
    while(1) {
        uint8_t op, pin;
        uint32_t addr, val;

        // all op's are uint8s to save space
        op = get_byte();
        switch(op) {
        case PI_REBOOT: 
            printk("PI: reboot\n");
            delay_ms(10);  // XXX: change uart-int to have a flush routine.
            clean_reboot();
        /*
         * Ignoring race conditions with other connections, 
         * we only really need put32 and get32 and can then
         * control everything from the unix side.  however,
         * it can be faster and easier to handle concurrent
         * access if we raise the level.  (If you read the 
         * NFS paper from cs240 you can see how it uses this
         * trick to handle distributed file system accesses.)
         */
        case PI_GET32:
            addr = get_uint32();
            val = GET32(addr);
            put_byte(PI_GET32_REPLY);
            put_uint32(val);
            break;
        case PI_PUT32:
            addr = get_uint32();
            val = get_uint32();
            PUT32(addr,val);
            break;

        /************************************************
         * for this part: add these opcodes.
         */
        case PI_GPIO_SET_INPUT:
			pin = get_byte();
			gpio_set_input(pin);
			break;
        case PI_GPIO_READ:
			pin = get_byte();
			val = gpio_read(pin);
			put_byte(PI_GPIO_READ_REPLY);
			put_uint32(val);
			break;
        case PI_GPIO_SET_OUTPUT:
			pin = get_byte();
			gpio_set_output(pin);
			break;
        case PI_GPIO_WRITE:
            pin = get_byte();
			val = get_uint32();
			gpio_write(pin, val);
            break;
		case PI_GPIO_SET_ON:
			pin = get_byte();
			gpio_set_on(pin);
			break;
		case PI_GPIO_SET_OFF:
			pin = get_byte();
			gpio_set_off(pin);
			break;
        default: 
            panic("unexpected op: %d\n", op);
        }
    }
}
