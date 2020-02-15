// implement:
//  void uart_init(void)
//
//  int uart_can_getc(void);
//  int uart_getc(void);
//
//  int uart_can_putc(void);
//  void uart_putc(unsigned c);
//
// see that hello world works.
//
//
#include "rpi.h"

// called first to setup uart to 8n1 115200  baud,
// no interrupts.
//  - you will need memory barriers, use <dev_barrier()>
//
//  later: should add an init that takes a baud rate.

// Auxiliary Module Registers
volatile unsigned* AUX_IRQ = (void*) 0x20215000;
volatile unsigned* AUX_ENABLES = (void*) 0x20215004;

// UART Registers
volatile unsigned* AUX_MU_IO_REG = (void*) 0x20215040;
volatile unsigned* AUX_MU_IER_REG = (void*) 0x20215044;
volatile unsigned* AUX_MU_IIR_REG = (void*) 0x20215048;
volatile unsigned* AUX_MU_LCR_REG = (void*) 0x2021504C;
volatile unsigned* AUX_MU_MCR_REG = (void*) 0x20215050;
volatile unsigned* AUX_MU_LSR_REG = (void*) 0x20215054;
volatile unsigned* AUX_MU_MSR_REG = (void*) 0x20215058;
volatile unsigned* AUX_MU_SCRATCH = (void*) 0x2021505C;
volatile unsigned* AUX_MU_CNTL_REG = (void*) 0x20215060;
volatile unsigned* AUX_MU_STAT_REG = (void*) 0x20215064;
volatile unsigned* AUX_MU_BAUD_REG = (void*) 0x20215068;

// UART Pins
#define UART0_TX 14
#define UART0_RX 15

#define DISABLE_UART 0x0
#define ENABLE_UART 0x1
#define CLEAR_TX_FIFO 0x2
#define CLEAR_RX_FIFO 0x4
#define ENABLE_8BIT_MODE 0x3
#define ENABLE_TXER 0x2
#define ENABLE_RXER 0x1
#define BAUD_115200 270
#define RX_READY 0x1
#define TX_READY 0x10
#define TX_EMPTY 0x20

// note: if you need memory barriers, use <dev_barrier()>
void uart_init(void) {
    
	dev_barrier();

	// set GPIO 14 and 15 to alternate function 0
    //volatile unsigned* gpio_reg = (void*) 0x20200004;
    //unsigned bitmask = get32(gpio_reg);
    gpio_set_function(14, GPIO_FUNC_ALT5);
	gpio_set_function(15, GPIO_FUNC_ALT5);
	//bitmask &= ~(0b111 << ((UART0_TX % 10) * 3));
    //bitmask &= ~(0b111 << ((UART0_RX % 10) * 3));
    //bitmask |= (0b010 << ((UART0_TX % 10) * 3));
    //bitmask |= (0b010 << ((UART0_RX % 10) * 3));
    //put32(gpio_reg, bitmask);

	dev_barrier();

    put32(AUX_ENABLES, get32(AUX_ENABLES) | (ENABLE_UART));
	
	dev_barrier();
	
    put32(AUX_MU_CNTL_REG, 0x0);
    put32(AUX_MU_IER_REG, 0x0);
    put32(AUX_MU_IIR_REG, CLEAR_RX_FIFO | CLEAR_TX_FIFO);
	put32(AUX_MU_LCR_REG, ENABLE_8BIT_MODE);
    put32(AUX_MU_BAUD_REG, BAUD_115200);
    put32(AUX_MU_CNTL_REG, ENABLE_TXER | ENABLE_RXER);

	dev_barrier();
}

// 1 = at least one byte on rx queue, 0 otherwise
int uart_can_getc(void) {
    return get32(AUX_MU_LSR_REG) & RX_READY;
}

// returns one byte from the rx queue, if needed
// blocks until there is one.
int uart_getc(void) {
    while(1)
        if(uart_can_getc())  break;
    return get32(AUX_MU_IO_REG) & 0xFF;
}

// 1 = space to put at least one byte, 0 otherwise.
int uart_can_putc(void) {
    return get32(AUX_MU_LSR_REG) & TX_EMPTY;
}

// put one byte on the tx qqueue, if needed, blocks
// until TX has space.
void uart_putc(unsigned c) {
    while(1)
            if(uart_can_putc())  break;
    put32(AUX_MU_IO_REG, c);
}


// simple wrapper routines useful later.

// a maybe-more intuitive name for clients.
int uart_has_data(void) {
    return uart_can_getc();
}

int uart_getc_async(void) { 
    if(!uart_has_data())
        return -1;
    return uart_getc();
}
