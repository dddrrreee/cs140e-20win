/*
 * engler, cs140e: simple tests to check that you are handling rising and falling
 * edge interrupts correctly.
 *
 * NOTE: make sure you connect your GPIO 20 to your GPIO 21 (i.e., have it "loopback")
 */
#include "rpi.h"
#include "timer-interrupt.h"
#include "libc/circular.h"
#include "sw-uart.h"
#include "cycle-util.h"

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
#define ENABLE_RX_INT 0x1
#define ENABLE_TX_INT 0x2

static volatile int num_tx;
static volatile int ninterrupt;
static  cq_t putQ;
static  cq_t getQ;
static volatile unsigned timeout = 0;

int is_uart_int(void) {
    unsigned int_status = get32(AUX_MU_IIR_REG);
    if(int_status & 0x1) {
        return 1;
    }
    return 0;
}

int uart_can_getc_int(void) {
	if(!cq_full(&getQ)) {
		return 1;
	}
	return 0;
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
// client has to define this.
// DON'T PUT BARRIERS IN ISR; CAUSES ISSUES
static unsigned char in_char = 0;
static unsigned char in_data = 0;
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

void uart_init_int(void) {
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
	
	// For some reason, the UART needs to have something
	// in its queue before I enable interrupts or things break...
	//put32(AUX_MU_IO_REG, '\r');
}

void mod_tx_interrupts (unsigned state) {
	dev_barrier();
	unsigned reg_val = get32(AUX_MU_IER_REG);
	reg_val |= 1;
	if(state) {
		put32(AUX_MU_IER_REG, reg_val);
	} else {
		// FIX 2: put32(AUX_MU_IO_REG, reg_val & ~1) doesn't clear bit as expected...
		put32(AUX_MU_IER_REG, 0);
		put32(AUX_MU_IER_REG, ENABLE_RX_INT);
	}
	dev_barrier();
}

unsigned char uart_getc_int(void) {
	
	unsigned char in_c = 130;
	if(!cq_empty(&getQ)) {
		cq_pop_nonblock(&getQ, &in_c);
	} 
	return in_c; //change to unsigned code? 
}

// Disable interrupts, push to HW UART TX
// Then re-enable interrupts

static volatile int debug_flag = 0;
void notmain() {
	
	sw_uart = sw_uart_init(21, 20, 115200);

	sw_uart_putc(&sw_uart, 'R');
	sw_uart_putc(&sw_uart, 'E');
	sw_uart_putc(&sw_uart, 'A');
	sw_uart_putc(&sw_uart, 'D');
	sw_uart_putc(&sw_uart, 'Y');
	sw_uart_putc(&sw_uart, '\n');

	int_init();

	uart_init_int();
	
	enable_cache();
    
	//sw_uart_printk(&sw_uart, "Initializing...\n");
	
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

    // If we want to enable system interrupts before something
	// is in the UART fifo, we need to have the handler output
	// a character...
	system_enable_interrupts();	

	
	//uart_putc_int('A');
	//sw_uart_putc(&sw_uart, uart_getc_int());
	
	//system_disable_interrupts();
	dev_barrier();	
	
	uart_putc_int('T');
	uart_putc_int('Y');
	uart_putc_int('P');
	uart_putc_int('E');
	uart_putc_int(' ');
	uart_putc_int('H');
	uart_putc_int('E');
	uart_putc_int('R');
	uart_putc_int('E');
	uart_putc_int('\n');
	
	dev_barrier();
	
	sw_uart_putc(&sw_uart, 'R');
	sw_uart_putc(&sw_uart, 'E');
	sw_uart_putc(&sw_uart, 'A');
	sw_uart_putc(&sw_uart, 'D');
	sw_uart_putc(&sw_uart, '\n');
	
	dev_barrier();
	
	for(;;) {
		unsigned char c = uart_getc_int();
		dev_barrier();
		if(c != 130) {
			uart_putc_int('#');
			dev_barrier();
			sw_uart_putc(&sw_uart, c);
		}
	}

    clean_reboot();
}
