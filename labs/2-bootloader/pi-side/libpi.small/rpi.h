#ifndef __SMALL_RPI_H__
#define __SMALL_RPI_H__
// engler, cs140e
// helper routine definitions.

// reboot the pi: call this so you don't have to pull the ttyserial
// out/in to hard reset it.
void reboot(void);

// jump to <addr>: note, will return to the caller
// of BRANCHTO.  is defined in the file <start.s>
void BRANCHTO(unsigned addr);

// get current value of micro-second counter.
unsigned timer_get_usec(void);

// block for <ms> milliseconds.
void delay_ms(unsigned ms);

/****************************************************************
 * simple memory read/write routines.
 */

// write the 32-bit value <v> to address <addr>
//  *(unsigned *)addr = v;
void PUT32(unsigned addr, unsigned v);
void put32(volatile void *addr, unsigned v);

// write the 8-bite value <v> to address <addr>
// *(unsigned char *)addr = v;
void PUT8(unsigned addr, unsigned v);

// read and return the 32-bit value at address <addr>
//  *(unsigned *)addr
unsigned GET32(unsigned addr);
unsigned get32(const volatile void *addr);


/****************************************************************
 * the UART hardware on the pi sends/receives bytes from the 
 * ttyserial we have plugged into it.
 * 
 * like most hardware: you have to initialize first.  then
 * you can read/write.
 */

// initialize UART.  MUST BE CALLED or bad things
// will happen.
void uart_init(void);

// read 1 byte from UART.
int uart_getc(void);

// write 1 byte to UART
void uart_putc(unsigned c);

// does UART have data available?
int uart_rx_has_data(void);

// null routine you can call to defeat compiler.
void dummy(unsigned);

#endif
