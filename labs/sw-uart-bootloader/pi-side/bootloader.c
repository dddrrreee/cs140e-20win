/* engler, cs140e.
 *
 * very simple bootloader.  more robust than xmodem, which seems to 
 * have bugs in terms of recovery with inopportune timeouts.
 */

/**********************************************************************
 * the code below is starter code provided by us.    the code you
 * should modify follows it (`notmain`).
 *
 *      DO NOT MODIFY THE FOLLOWING!!!
 *      DO NOT MODIFY THE FOLLOWING!!!
 *      DO NOT MODIFY THE FOLLOWING!!!
 *      DO NOT MODIFY THE FOLLOWING!!!
 *      DO NOT MODIFY THE FOLLOWING!!!
 */
#include "rpi.h"
#include "rpi-interrupts.h"
#include "libc/circular.h"
#include "sw-uart.h"
#include "cycle-util.h"

#define __SIMPLE_IMPL__
#include "../shared-code/simple-boot.h"         // holds crc32

//int sw_uart_putc_int(sw_uart_t *uart, int timeout_usec);
//int sw_uart_getc(sw_uart_t* uart);

//static cq_t putQ;
static sw_uart_t u;

static volatile int ninterrupt;

enum { out_pin = 21, in_pin = 20 };
static volatile unsigned n_rising_edge, n_falling_edge, n_interrupt;

// blocking calls to send / receive a single byte from the uart.
inline static void put_byte(unsigned char uc)  { 
    sw_uart_putc(&u, uc);
}
inline static unsigned char get_byte(void) { 
    return sw_uart_getc_int(&u, 100);
}

inline static unsigned get_uint(void) {
	unsigned u = get_byte();
        u |= get_byte() << 8;
        u |= get_byte() << 16;
        u |= get_byte() << 24;
	return u;
}
inline static void put_uint(unsigned u) {
    put_byte((u >> 0)  & 0xff);
    put_byte((u >> 8)  & 0xff);
    put_byte((u >> 16) & 0xff);
    put_byte((u >> 24) & 0xff);
}

// send the unix side an error code and reboot.
static void die(int code) {
    put_uint(code);
    clean_reboot();
}

// send a string <msg> to the unix side to print.  
// message format:
//  [PRINT_STRING, strlen(msg), <msg>]
//
// DANGER DANGER DANGER!!!  Be careful WHEN you call this!
// DANGER DANGER DANGER!!!  Be careful WHEN you call this!
// DANGER DANGER DANGER!!!  Be careful WHEN you call this!
//
// We do not have interrupts and the UART can only hold 
// 8 bytes before it starts dropping them.   so if you print at the
// same time the UNIX end can send you data, you will likely
// lose some, leading to weird bugs.  If you print, safest is 
// to do right after you have completely received a message.
void sw_putk(const char *msg) {
    put_uint(PRINT_STRING);
    // lame strlen.
    int n;
    for(n = 0; msg[n]; n++)
        ;
    put_uint(n);
    for(n = 0; msg[n]; n++)
        put_byte(msg[n]);
}

// send a <GET_PROG_INFO> message every 300ms.
// 
// NOTE: look at the idiom for comparing the current usec count to 
// when we started.  
void wait_for_data(void) {
    /*
	int data_in = -1;
    while(data_in == -1) {
        put_uint(GET_PROG_INFO);
		unsigned time_start = timer_get_usec_raw();
		unsigned time_end = time_start;
		while(time_end - time_start < 300 *1000) {
			data_in = sw_uart_getc(&u);
			time_end = timer_get_usec_raw();
            if (data_in != -1) {
                break;
            }
		}
    } */
    
	/*
	while(1) {
        put_uint(GET_PROG_INFO);

        unsigned s = timer_get_usec();
        // the funny subtraction is to prevent wrapping.
        while((timer_get_usec() - s) < 300*1000) {
            // the UART says there is data: start eating it!
            //if(uart_rx_has_data())
            //    return;
        }
    } */
	
	while(1) {
        put_uint(GET_PROG_INFO);

        unsigned s = timer_get_usec();
        // the funny subtraction is to prevent wrapping.
        while((timer_get_usec() - s) < 300*1000) {
            // the UART says there is data: start eating it!
        }
    }
}

/*****************************************************************
 * All the code you write goes below.
 */

#define CODE_LIMIT 0x200000

// Simple bootloader: put all of your code here: implement steps 2,3,4,5,6
void notmain(void) {
    u = sw_uart_init(21, 20, 115200);
    
    enable_cache();
    
    //int_init();
    
    //gpio_int_falling_edge(in_pin);
    
    //cq_init(&putQ, 1);
    
    //system_enable_interrupts();
    
    // 1. keep sending GET_PROG_INFO until there is data.
    wait_for_data();

    /****************************************************************
     * Add your code below: 2,3,4,5,6
     */


    // 2. expect: [PUT_PROG_INFO, addr, nbytes, cksum] 
    //    we echo cksum back in step 4 to help debugging.
	unsigned in_data = sw_uart_getc_int(&u, 100);
        in_data |= sw_uart_getc_int(&u, 100) << 8;
        in_data |= sw_uart_getc_int(&u, 100) << 16;
        in_data |= sw_uart_getc_int(&u, 100)<< 24;
	if(in_data != PUT_PROG_INFO) {
		die(BOOT_ERROR);
	}
	unsigned addr = get_uint();
	unsigned nbytes = get_uint();
	unsigned cksum = get_uint();
    
	// 3. If the binary will collide with us, abort. 
    //    you can assume that code must be below where the booloader code
    //    gap starts.
	
	// Disassembly shows address of BRANCHAT is  0x0020000C, but stop by 0x00200000
	if(addr + nbytes > (unsigned) BRANCHTO) {
		die(BAD_CODE_ADDR);
	}

    // 4. send [GET_CODE, cksum] back.
	put_uint(GET_CODE);
	put_uint(cksum);
    
	// 5. expect: [PUT_CODE, <code>]
    //  read each sent byte and write it starting at 
    //  ARMBASE using PUT8
	if (get_uint() != PUT_CODE) {
		die(BOOT_ERROR);
	}
	int i = 0;
	for(; i < nbytes; i++) {
		PUT8(addr + i, get_byte());
	}

    // 6. verify the cksum of the copied code.
	if(cksum != crc32((void*) addr, nbytes)) {
		put_uint(BAD_CODE_CKSUM);
	}

    /****************************************************************
     * add your code above: don't modify below unless you want to 
     * experiment.
     */

    // 7. no previous failures: send back a BOOT_SUCCESS!
    put_uint(BOOT_SUCCESS);

	// XXX: appears we need these delays or the unix side gets confused.
	// I believe it's b/c the code we call re-initializes the uart; could
	// disable that to make it a bit more clean.
	//
    // XXX: I think we can remove this now; should test.
    delay_ms(500);

    // run what we got.
    BRANCHTO(ARMBASE);

    // should not get back here, but just in case.
    clean_reboot();
}
