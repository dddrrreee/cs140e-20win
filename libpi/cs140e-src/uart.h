#ifndef __UART_H__
#define __UART_H__

// 2.1, p8
// XXX: go through and do the bitfields for these.
struct aux_periphs {
    volatile unsigned
        /* <aux_mu_> regs */
        io,     // p11
        ier,

#       define CLEAR_TX_FIFO    (1 << 1)
#       define CLEAR_RX_FIFO    (1 << 2)
#       define CLEAR_FIFOs  (CLEAR_TX_FIFO|CLEAR_RX_FIFO)
        // dwelch does not write the low bit?
#       define IIR_RESET        ((0b11 << 6) | 1)
        iir,

        lcr,
        mcr,
        lsr,
        msr,
        scratch,

#       define RX_ENABLE (1 << 0)
#       define TX_ENABLE (1 << 1)
        cntl,

        stat,
        baud;
};

// never use this directly!
static inline struct aux_periphs *uart_get(void) { return (void*)0x20215040; }

#endif
