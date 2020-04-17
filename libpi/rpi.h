/*
 * General functions we use.  These could be broken into multiple small
 * header files, but that's kind of annoying to context-switch through,
 * so we put all the main ones here.
 */
#ifndef __RPI_H__
#define __RPI_H__

#define RPI_COMPILED

// We are running without an OS, but these will get pulled from gcc's include's,
// not your laptops.
// 
// however, we don't want to do this too much, since unfortunately header files
// have a bunch of code we cannot run, which can lead to problems.
//
// XXX: These are dangerous since we are not doing any initialization (e.g., of
// locale).  
#include <stddef.h>
#include <stdint.h>
// XXX: probably should not do this --- alot of random code in <string.h>
#include <string.h>

/*****************************************************************************
 * standard libc like functions for the pi.
 */

// change these two function pointers to control where pi output goes.
extern int (*rpi_putchar)(int c);
void rpi_reset_putc(void);
void rpi_set_putc(int (*fp)(int));

// int putk(const char *msg);
extern int (*putk)(const char *p);

// call to change output function pointers.
void rpi_set_output(int (*putc_fp)(int), int (*puts_fp)(const char *));


// this doesn/t work b/c we use %b for bits.
int printk(const char *format, ...);
//                 __attribute__ ((format (printf, 1, 2)));
int snprintk(char *buf, size_t n, const char *fmt, ...);
                // __attribute__ ((format (printf, 3, 4)));

int uart_hex(unsigned h);

// a not very good rand()
unsigned short rpi_rand16(void);
unsigned long rpi_rand32(void);
void rpi_reset(unsigned seed);

/*****************************************************************************
 * common device functions
 */

// uart functions
void uart_init(void);
int uart_getc(void);
void uart_putc(unsigned c);

// returns -1 if no byte, the value otherwise.
int uart_getc_async(void);

// 0 = no data, 1 = at least one byte
int uart_has_data(void);
// 0 = no space, 1 = space for at least 1 byte
int uart_can_putc(void);

/***************************************************************************
 * simple timer functions.
 */

// delays for <ticks> (each tick = a few cycles)
void delay_cycles(unsigned ticks) ;
// delay for <us> microseconds.
void delay_us(unsigned us) ;
// delay for <ms> milliseconds
void delay_ms(unsigned ms) ;

// returns time in usec.
// NOTE: this can wrap around!   do not do direct comparisons.
// this does a memory barrier.
unsigned timer_get_usec(void) ;
// this does no memory barrier.
unsigned timer_get_usec_raw(void);

 
// returns time in nanosec.
// NOTE: this can wrap around!   do not do direct comparisons.
unsigned timer_get_ns(void) ;

/****************************************************************************
 * Reboot the pi smoothly.
 */

// reboot the pi.
void rpi_reboot(void) __attribute__((noreturn));

// reboot after printing out a string to cause the unix my-install to shut down.
void clean_reboot(void) __attribute__((noreturn));

/*******************************************************************************
 * simple memory allocation: no free, just have to reboot().
 */

// returns 0-filled memory.
void *kmalloc(unsigned nbytes) ;
void *kmalloc_aligned(unsigned nbytes, unsigned alignment);

// initialize kmalloc
void kmalloc_init(void);
// initialize and set where the heap starts.
void kmalloc_init_set_start(unsigned _addr);

// return pointer to the first free byte.  used for
// bounds checking.
void *kmalloc_heap_ptr(void);

// currently no-ops.
void kfree(void *p);
void kfree_all(void);

// does [p, p+n) only contain 0s?
int memiszero(const void *_p, unsigned n);

/*****************************************************************************
 * memory related helpers
 */

// memory barrier.
void dmb(void);
// sort-of write memory barrier (more thorough).  dsb() >> dmb().
void dsb(void);

// use this if you need a device memory barrier.
void dev_barrier(void);
#define mb dev_barrier

// cache enable
void enable_cache(void) ;
void disable_cache(void) ;
void flush_all_caches(void);


unsigned rpi_get_model(void);
int rpi_get_memsize(unsigned *s);
int rpi_get_serial(unsigned s[2]);

/*****************************************************************************
 * Low-level code: you could do in C, but these are in assembly to defeat
 * the compiler.
 */
// *(unsigned *)addr = v;
void PUT32(unsigned addr, unsigned v);
void put32(volatile void *addr, unsigned v);

// *(unsigned char *)addr = v;
void PUT16(unsigned addr, unsigned short v);
void put16(volatile void *addr, unsigned short v);

// *(unsigned char *)addr = v;
void PUT8(unsigned addr, unsigned char v);
void put8(volatile void *addr, unsigned char v);

// *(unsigned *)addr
unsigned GET32(unsigned addr);
unsigned (get32)(const volatile void *addr);
#if 0
#define get32(addr) ({      \
    _Static_assert( sizeof(typeof(* (addr))) == 4, "underlying type not 32bits");\
    (get32)(addr);          \
})
#endif


// *(unsigned short *)addr
unsigned short GET16(unsigned addr);
unsigned short get16(const volatile void *addr);

// *(unsigned char *)addr
unsigned char GET8(unsigned addr);
unsigned char get8(const volatile void *addr);

// jump to <addr>
void BRANCHTO(unsigned addr);

// a no-op routine called to defeat the compiler.
void dummy(unsigned);

// for initializing using the control block.
void cb_init(void);


// simple bootloader: given a channel, loads the code,
// then returns address to jump to.
// could return an error if failed, right now just panic.
struct vdev;
uint32_t load_code(struct vdev *v);

#include "cs140e-src/gpio.h"

// extra student-defined libc prototypes.
#include "libc/libc-extra.h"
#ifndef RPI_UNIX
#   include "libc/assert.h"
#else
#   define PUT32(addr,val) put32((void*)(unsigned long)addr, val)
#   define GET32(addr) get32((void*)(unsigned long)addr)
#   include "demand.h"
#   include <assert.h>
#endif

// entry point definition
void notmain(void);


/******************************************************************
 * SD card routines.
 */

// read sector [lba,lba+nsec) into <buf>.   must be large enough. 
// returns 0 on error, <nsec> read in on not.
int pi_sd_read(void *data, uint32_t lba, uint32_t nsec);

// called to init first.
int pi_sd_init(void);

// read and allocate a buffer entry <nsec*secsize> big
void *pi_sec_read(uint32_t lba, uint32_t nsec);

#endif
