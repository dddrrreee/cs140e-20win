#ifndef __CQ_H__
#define __CQ_H__

// simple lock-free circular queue.
#ifndef LINUX
#   include "rpi.h"
#else
#   define printk printf
#   include <assert.h>
#   include <stdio.h>
#   include <stdlib.h>
#   include "demand.h"
#endif

// simple circular FIFO Q.   
//
// if we want to store records?
//
// TODO:
//  - possibly should just include all the code in the header so it gets inlined.
//  - also should extend so it works with threads.
//  - should symex test this.   "How to write code to execute on a symbolic CPU"
//
// could simplify the interface if we wanted to only allow < 32bit valuesto be stored.

// make the code templatized on this? (ala fraser and hanson)  
typedef unsigned char cqe_t;

// XXX: i think change this so you can size the buffers to what you think makes sense.
// e.g., for output we probably want a huge one.  or dynamically adapt.   if you want
// stack allocation they could pass in pointers to the buffer, etc.
//
// hardcoding N means we can stack allocate everything if needed.  (avoiding dynamic
// allocation seems like something worthwhile in an embedded system, but in our case
// perhaps is not relevant since we have the trivial kmalloc pointer bump.)
#ifndef CQ_N
#   define CQ_N 8192
#endif
typedef struct {
    // single mutator of head, single mutator of tail = lock free.  
    volatile cqe_t c_buf[CQ_N];
    unsigned fence;
    volatile unsigned head, tail;

    // number of times a push failed b/c there were more than N elements.
    volatile unsigned overflow;
    // do we panic on error?
    unsigned errors_fatal_p:1;
} cq_t;

void cq_init(cq_t *q, unsigned errors_fatal_p);

static inline int cq_empty(cq_t *q);
static inline int cq_full(cq_t *q);

// returns 0 if no space.
static inline int cq_push(cq_t *q, cqe_t e);

// unimplemented: yields cpu / spins until there is space.
static inline void cq_push_blocking(cq_t *q, cqe_t e);

static inline int cq_peek(cq_t *q, cqe_t *e);

// FIFO pop: blocks until there is an element.
cqe_t cq_pop(cq_t *q);

void cq_pop_n(cq_t *q, void *data, unsigned n);

int cq_pop_n_noblk(cq_t *q, void *data, unsigned n);

int cq_push_n(cq_t *q, void *data, unsigned n);

// FIFO pop, non-blocking: 0 on fail, 1 on success (and e is written to)
static inline int cq_pop_nonblock(cq_t *q, cqe_t *e);

void cq_print(cq_t *c);

/*************************************************************************************
* inline these so they can be used when we bit-bang off the UART with no interrupts.
*/
static inline int cq_empty(cq_t *q) { return q->head == q->tail; }

// if adding 1 to head = tail, then we have no space.
static inline int cq_full(cq_t *q) { return (q->head+1) % CQ_N == q->tail; }

static inline unsigned cq_nelem(cq_t *q) { return (q->head - q->tail) % CQ_N; }
static inline unsigned cq_nspace(cq_t *q) { return CQ_N - cq_nelem(q); }

// not blocking: wait: this never calls getc?
static inline int cq_pop_nonblock(cq_t *c, cqe_t *e) {
    if(cq_empty(c))
        return 0;
    unsigned tail = c->tail;
    *e = c->c_buf[tail];
    c->tail = (tail+1)%CQ_N;
    return 1;
}

// non-blocking push.
static inline int cq_push(cq_t *c, cqe_t x) {
    unsigned head = c->head;
    unsigned tail = c->tail;
    if(cq_full(c)) 
        return 0;
    c->c_buf[head] = x;
    c->head = (head + 1) % CQ_N;
    return 1;
}

static inline int cq_peek(cq_t *c, cqe_t *e) {
    if(cq_empty(c))
        return 0;
    *e = c->c_buf[c->tail];
    return 1;
}

// non-destructively peek ahead <n> entries.
int cq_peek_n(cq_t *c, cqe_t *v, unsigned n);
#endif
