/*
 * engler, cs140e: simple echo program to test that you are doing uart interrupts
 * correctly.
 */
#include "rpi.h"
#include "timer-interrupt.h"
#include "libc/circular.h"
#include "sw-uart.h"
#include "cycle-count.h"
#include "uart-int.h"

// Auxiliary Module Registers
static volatile unsigned* AUX_IRQ = (void*) 0x20215000;
static volatile unsigned* AUX_ENABLES = (void*) 0x20215004;

// UART Registers
static volatile unsigned* AUX_MU_IO_REG = (void*) 0x20215040;
static volatile unsigned* AUX_MU_IER_REG = (void*) 0x20215044;
static volatile unsigned* AUX_MU_IIR_REG = (void*) 0x20215048;
static volatile unsigned* AUX_MU_LCR_REG = (void*) 0x2021504C;
static volatile unsigned* AUX_MU_MCR_REG = (void*) 0x20215050;
static volatile unsigned* AUX_MU_LSR_REG = (void*) 0x20215054;
static volatile unsigned* AUX_MU_MSR_REG = (void*) 0x20215058;
static volatile unsigned* AUX_MU_SCRATCH = (void*) 0x2021505C;
static volatile unsigned* AUX_MU_CNTL_REG = (void*) 0x20215060;
static volatile unsigned* AUX_MU_STAT_REG = (void*) 0x20215064;
static volatile unsigned* AUX_MU_BAUD_REG = (void*) 0x20215068;
static volatile unsigned* IRQ_ENABLE_1 = (void*)0x2000B210;
static volatile unsigned* IRQ_ENABLE_2 = (void*) 0x2000B214;
static volatile unsigned tx_count = 0;

void mod_tx_interrupts (unsigned state);
static sw_uart_t sw_uart;
static  cq_t putQ;
static  cq_t getQ;

// UART Pins
#define UART0_TX 14
#define UART0_RX 15

#define DISABLE_UART 0x0
#define ENABLE_UART 0x1
#define ENABLE_8BIT_MODE 0x3
#define ENABLE_TXER 0x2
#define ENABLE_RXER 0x1
#define BAUD_115200 270
#define RX_READY 0x1
#define TX_READY 0x10
#define TX_EMPTY 0x20
#define ENABLE_RX_INT 0x1
#define ENABLE_TX_INT 0x2

int uart_has_data_int(void){
    return !cq_empty(&getQ);
}

void mod_tx_interrupts (unsigned state) {
    dev_barrier();
    unsigned reg_val = get32(AUX_MU_IER_REG);
    if(state) {
        put32(AUX_MU_IER_REG, reg_val | ENABLE_TX_INT);
    } else {
        // FIX 2: put32(AUX_MU_IO_REG, reg_val & ~1) doesn't clear bit as expected...
        put32(AUX_MU_IER_REG, reg_val & ~ENABLE_TX_INT);
    }
    dev_barrier();
}

static unsigned char in_char = 0;
static unsigned char in_data = 0;

// client has to define this.
void interrupt_vector(unsigned pc) {
        // See note below
    // Receive case
    unsigned reg_val = get32(AUX_MU_IIR_REG);
    if((reg_val & 0x4) > 0) {
        // FIX 1: DON'T ASSIGN OUTPUT OF UART_CAN_GETC TO IN_CHAR
        while(!cq_full(&getQ) && (uart_can_getc())) {
            unsigned in_char = uart_getc();
            //sw_uart_putc(&sw_uart, (char)in_char);
            cq_push(&getQ, in_char);
        }
    }
    // Transmit case
    else if ((reg_val & 0x2) > 0) {
        while((uart_can_putc()) && cq_pop_nonblock(&putQ, &in_data)) {
            uart_putc(in_data);
        }

        if(cq_empty(&putQ)) {
            mod_tx_interrupts(0);
        }
    }
}

void uart_putc_int(unsigned char c) {
    system_disable_interrupts();
    cq_push(&putQ, c);
    mod_tx_interrupts(1);
    
    unsigned char d;
    while(uart_can_putc() && cq_pop_nonblock(&putQ, &d)) {
        uart_putc(d);
    }

    system_enable_interrupts();
}

int uart_getc_int(void) {
    //mod_tx_interrupts(0);
    system_disable_interrupts();
	unsigned char in_c = 130;
    if(!cq_empty(&getQ)) {
        cq_pop_nonblock(&getQ, &in_c);
    }
	system_enable_interrupts();
    //mod_tx_interrupts(1);
    return in_c; //change to unsigned code?
}

void my_puts(const char *str) {
    for(; *str; str++)
        uart_putc_int(*str);
}

void uart_init_with_interrupts(void) {
    dev_barrier();

    // set GPIO 14 and 15 to alternate function 0
    gpio_set_function(14, GPIO_FUNC_ALT5);
    gpio_set_function(15, GPIO_FUNC_ALT5);

    dev_barrier();

    put32(AUX_ENABLES, get32(AUX_ENABLES) | (ENABLE_UART));
    
    dev_barrier();
    
    put32(AUX_MU_CNTL_REG, 0x0);
    put32(AUX_MU_IER_REG, ENABLE_TX_INT | ENABLE_RX_INT);
    put32(AUX_MU_IIR_REG, CLEAR_RX_FIFO | CLEAR_TX_FIFO);
    put32(AUX_MU_LCR_REG, ENABLE_8BIT_MODE);
    put32(AUX_MU_BAUD_REG, BAUD_115200);
    put32(AUX_MU_CNTL_REG, ENABLE_TXER | ENABLE_RXER);
    
    // Using mini-UART, so we need to use the AUX interrupt
    put32(IRQ_ENABLE_1, 1 << 29);

    dev_barrier();
    
}

void notmain() {
    int_init();
    uart_init_with_interrupts();
    
    cq_init(&getQ, 1);
    cq_init(&putQ, 1);
     
      //dev_barrier();
    
      // For some reason, the UART needs to have something
      // in its queue before I enable interrupts or things break...
      //
      // This statement also has to be AFTER cache is enabled
      // and queue is initialized...
    put32(AUX_MU_IO_REG, '\r');
      
    dev_barrier();
    
    system_enable_interrupts();

	dev_barrier();

#if 0
	// easiest test: just use the hw-uart.
	my_puts("going to test using the hw-uart only:\n");
	while(1) {
        while(uart_has_data_int())
            uart_putc_int(uart_getc_int());
    }
#endif 

#if 0
    // slightly more fancy: make sure printk works.
    my_puts("going to test using printk\n");
    // overwrite printk's putc method so that it calls our putk.
    rpi_set_putc(uart_putc_int);
    while(1) {
        while(uart_has_data_int()) 
            printk("%c", uart_getc_int());
    }
#endif

    /* 
     * this tests using the sw-uart: note we likely have to 
     * disable interrupts to make sure the sw-uart can do its
     * timings.  however, if you tune your hw-uart interrupt 
     * handler you might be able to get rid of such tricks.
     *
     * if you aren't using the sw-bootloader need to setup the 
     * sw uart first.
     * 
     * you'll have to have pi-cat running in another window.
     */ 
    unsigned tx = 20, rx = 21;
    sw_uart_t s = sw_uart_init(tx,rx, 115200);

    printk("enter input with a carriage return\n");
    // do 10 lines.
    for(int i = 0; i < 10; ) {
        while(uart_has_data_int()) {
            int c = uart_getc_int();
            system_disable_interrupts();
            sw_uart_putc(&s, c);
            system_enable_interrupts();
            uart_putc_int(c);
            if(c == '\n')
                i++;
        }
    }
    printk("Done: handled %d lines\n");

    printk("SUCCESS!\n");
    clean_reboot();
}

