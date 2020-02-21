/*
 * engler, cs140e: simple echo program to test that you are doing uart interrupts
 * correctly.
 */

/* Interrupt-based UART code for HW1 of CS140E by Brian Jun */

// Module includes
#include "rpi.h"
#include "timer-interrupt.h"
#include "libc/circular.h"
#include "sw-uart.h"
#include "cycle-count.h"
#include "uart-int.h"

// Auxiliary Module Registers
// Thse registers are defined on 9 of the Broadcom datqsheet.
// The AUX IRQ register indicates whether there is an interrupt pending
// for the AUX module. In this case, we check for an interrupt on the mini-UART.
static volatile unsigned* AUX_IRQ = (void*) 0x20215000;

// The AUX ENB register allows peripherals of the AUX module to be enabled
// by the code; we must enable the mini-UART.
static volatile unsigned* AUX_ENABLES = (void*) 0x20215004;

// Mini UART Register Map
// Page 11 of the Broadcom datasheet defines the AUX MU IO reg, which
// is primarily used to read and write data to the mini-UART.
static volatile unsigned* AUX_MU_IO_REG = (void*) 0x20215040;

// IRQ register 
// Page 116 of the Broadcom datasheet defines the IRQ enable register, which
// we can use to initialize the AUX interrupt.
static volatile unsigned* IRQ_ENABLE_1 = (void*) 0x2000B210;

// Function prototypes for helper functions
void mod_tx_interrupts (unsigned state);

// Module-level variables

// Interrupt-driven HW UART
// Struct defined in uart.h
static hw_uart_t* hw_uart;

// Circular queues for transmission and reception
static  cq_t putQ;
static  cq_t getQ;

// Holder for the AUX IRQ register
static unsigned aux_irq_reg;

// UART0 Pins
#define UART0_TX 14
#define UART0_RX 15

// UART Status
#define DISABLE_UART 0x0
#define ENABLE_UART 0x1

// UART Configuration
#define ENABLE_8BIT_MODE 0x3
#define BAUD_115200 270
#define RX_READY 0x1
#define TX_READY 0x10
#define TX_EMPTY 0x20
#define ENABLE_RX_INT 0x1
#define ENABLE_TX_INT 0x2

/*
 * Function: uart_has_data_int
 * Parameters: none
 * Returns: whether receive queue has data
 */
int uart_has_data_int(void){
    return !cq_empty(&getQ);
}

/*
 * Function: uart_get_aux_irq
 * Parameters: none
 * Returns: update state of the AUX interrupt register
 *
 * Notes: where possible in this assignment, I attempted to adhere
 * to the function prototypes given in the header. Thus, some functions
 * will be invoked even if there is a performance penalty due to
 * my desire to adhere to starter code.
 *
 */
unsigned* const uart_get_aux_irq(void) {
	// According to page 9 of the Broadcom datasheet,
	// the AUX_IRQ register is defined at 0x20215000
	aux_irq_reg = get32(AUX_IRQ);
	return &aux_irq_reg;
}

/*
 * Function: uart_has_int
 * Parameters: hw_uart_t, pointer to the mini-UART struct
 * Returns: enum reflecting the state of the HW UART
 */
uart_int_t uart_has_int(hw_uart_t* hw_uart) {
	uart_get_aux_irq();
	// According to page 9 of the Broadcom datasheet,
	// the AUX_IRQ register is defined at 0x20215000
	// and the LSB is the IRQ status of the mini UART.
	// It is set if there's a pending interrupt, so 
	// we check with a bit mask of 0x1
	if((aux_irq_reg & 0x1) == 0) {
		return UART_NO_INT;
	}

	// According to page 13 of the Broadcom datasheet and errata,
	// the AUX_MU_IIR_REG register defines bit 2 being set 
	// as the presence of a receive interrupt and bit 1 being
	// set as the presence of a transmit interrupt
	unsigned reg_val = get32(&(hw_uart->iir));
	if((reg_val & 0x4) > 0) {
		return UART_RX_INT;
	} else if (reg_val & 0x2) {
		return UART_TX_INT;
	} else {
		return UART_NO_INT;
	}
}

/*
 * Function: mod_tx_interrupts
 * Parameters: state of TX interrupts: 1 for on and 0 for off
 * Returns: none
 */
void mod_tx_interrupts (unsigned state) {

	// Page 7 of the Broadcom manual requires pair of read and write
	// barriers between accesses to different peripherals. Because we
	// modify the mini-UART and are unclear if we have accessed another
	// peripheral previously, I place a set of barriers.
    dev_barrier();
    
	// According to page 12 of the Broadcom datasheet and errata,
	// the AUX_MU_IIR_REG register defines bit 0 being set 
	// as the presence of a receive interrupt and bit 1 being
	// set as the presence of a transmit interrupt
	unsigned reg_val = get32(&(hw_uart->ier));
    if(state) {
        put32(&(hw_uart->ier), reg_val | ENABLE_TX_INT);
    } else {
        put32(&(hw_uart->ier), reg_val & ~ENABLE_TX_INT);
    }

	// Reference above
    dev_barrier();
}

static unsigned char in_char = 0;
static unsigned char in_data = 0;

/*
 * Function: uart_interrupt_handler
 * Parameters: None
 * Returns: 1 on success
 *
 * Notes: This is not the most efficient implementation of the 
 * interrupt handler. To adhere to header prototypes, I check
 * the type of UART interrupt using uart_has_int, which restricts
 * it to TX, RX, or no interrupt. To make this more efficient, I 
 * could check both types of interrupts when the handler is invoked.
 *
 */
int uart_interrupt_handler(void) {
	
	// Page 7 of the Broadcom manual requires pair of read and write
	// barriers between accesses to different peripherals. Because we
	// modify the mini-UART and are unclear if we have accessed another
	// peripheral previously, I place a set of barriers.
	dev_barrier();

	// Check that mini-UART is interrupt source
	uart_int_t status = uart_has_int(hw_uart);

	// Handle TX and RX interrupts appropriately
	if(status == UART_RX_INT) {
		// If we have a receive interrupt, 
		// check we have space on the receive queue and that
		// the HW UART can receive a character.
		// If so, transfer character from HW to queue
        while(!cq_full(&getQ) && (uart_can_getc())) {
            unsigned in_char = uart_getc();
            cq_push(&getQ, in_char);
        }
	} else if (status == UART_TX_INT) {
		// If we have a transmit interrupt, 
		// check we have popped a valid character from the TX queue and that
		// the HW UART can transmit a character.
		// If so, transfer character from transmit queue to HW
       
		uart_int_flush_all();
		
		// Check that the transmit queue is not empty. Otherwise,
		// we will not get another set of interrupts
        if(cq_empty(&putQ)) {
            mod_tx_interrupts(0);
        }
	} else {
		;
	}

	dev_barrier();
	return 1;
}

// Interrupt vector to handle UART interrupts
void interrupt_vector(unsigned pc) {
	uart_interrupt_handler();	
}

/*
 * Function: uart_int_flush_all
 * Parameters: None
 * Returns: Nones
 */
void uart_int_flush_all(void){
	unsigned char d;
	// While UART HW can tranmit characters and we pop 
	// valid characters from the transmit queue, place
	// the character onto the hardware UART buffer
    while(uart_can_putc() && cq_pop_nonblock(&putQ, &d)) {
        uart_putc(d);
    }
}


/*
 * Function: uart_putc_int
 * Parameters: c: the character to transmit
 * Returns: the character being transmitted
 */
int uart_putc_int(int c) {

	// Push onto transmit queue
    cq_push(&putQ, c);
	
	// We must disable interrupts because we do not want
	// an interrupt to fire while we are modifying interrupt state.
    system_disable_interrupts();

	// If transmit interrupts had been disabled in handler 
	// due to lack of characters to transmit, that condition
	// can no longer hold if we want to transmit one. Thus,
	// re-enable transmit interrupts
    mod_tx_interrupts(1);

	// While the UART HW is able and the transmit queue has characters,
	// exhaust our transmit queue.
    uart_int_flush_all();

	// Done with interrupt modifications, so re-enable interrupts
    system_enable_interrupts();

	return c;
}

/*
 * Function: uart_getc_int
 * Parameters: none
 * Returns: the character we received
 */
int uart_getc_int(void) {

	// Initialize input character to invalid ASCII code so that 
	// even if pop fails, no extra character is output
	unsigned char in_c = 130;
    
	// Pop character from the receive queue
	if(!cq_empty(&getQ)) {
        cq_pop_nonblock(&getQ, &in_c);
    }

	// Finished with shared resource, so re-enable interrupts
    return in_c; 
}

/*
 * Function: my_puts
 * Parameters: string to output
 * Returns: none
 */
void my_puts(const char *str) {
    for(; *str; str++)
        uart_putc_int(*str);
}

/*
 * Function: uart_init_with_interrupts
 * Parameters: nonet
 * Returns: none
 */
void uart_init_with_interrupts(void) {
	// According to the Broadcom manual, we need a dsm and dmb barriers
	// between operations on different peripherals.
    dev_barrier();

   	// According to page 102 of the Broadcom manual, 
	// to get the mini-UART functional on pins 14 and 15,
	// we need to set them to alternate function 5
	gpio_set_function(14, GPIO_FUNC_ALT5);
    gpio_set_function(15, GPIO_FUNC_ALT5);

	// According to the Broadcom manual, we need a dsm and dmb barriers
	// between operations on different peripherals, in this case GPIO
	// and mini UART.
    dev_barrier();

    put32(AUX_ENABLES, get32(AUX_ENABLES) | (ENABLE_UART));
    
	// According to the Broadcom manual, we need a dsm and dmb barriers
	// between operations on different peripherals, in this case AUX
	// and mini UART.
    dev_barrier();
    
	// The Broadcom manual defines mini-UART registers on pages 11-17, 
	// and the code for setting it up has previously been verified 
	// via checksums
	hw_uart = (hw_uart_t*) AUX_MU_IO_REG; 
	put32(&(hw_uart->cntl), 0x0);

	// Page 12 of the Broadcom datasheet and errata show that bits 0 and 1
	// control whether the interrupts are enabled
	put32(&(hw_uart->ier), ENABLE_TX_INT | ENABLE_RX_INT);
	put32(&(hw_uart->iir), CLEAR_RX_FIFO | CLEAR_TX_FIFO);
	put32(&(hw_uart->lcr), ENABLE_8BIT_MODE);

	// Page 11 dictates the formula to calculate the baud rate of the mini-UART.
	// With 250MHz system clock and 115200 desired baud rate, the baud rate register
	// must be loaded with value 270.
	put32(&(hw_uart->baud), BAUD_115200);
	put32(&(hw_uart->cntl), TX_ENABLE | RX_ENABLE);

	// According to the Broadcom manual, we need a dsm and dmb barriers
	// between operations on different peripherals, in this case IRQ
	// and mini UART.
    dev_barrier();
    
	// Using mini-UART, so we need to use the AUX interrupt.
	// This slot is defined in page 113 of the Broadcom manual
    put32(IRQ_ENABLE_1, 1 << 29);

	// Initialize the transmit and receive queues
	cq_init(&getQ, 1);
    cq_init(&putQ, 1);

	dev_barrier();
}

void notmain() {
    int_init();
    uart_init_with_interrupts();
    system_enable_interrupts();

#if 1
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

#if 0
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
#endif
}




