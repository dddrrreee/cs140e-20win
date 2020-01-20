/*
 * Simple tracing memory implementation.  Used for cross-checking.
 * 
 * you will implement a simple fake memory that maps addresses to values.  
 * e.g., an array, linked list, hash table of (<addr>, <val>) tuples such 
 * that after you insert some (<addr>, <val>), you can lookup <val> using 
 * <addr> as a key.
 *
 * simplistic assumptions:
 *  - all loads/stores are 32 bits.
 *  - read returns value of last write, or random() if none.
 *  - load and store prints the addr,val
 * 
 * HINT: I woul suggest a fixed size array, that you do a simple linear scan 
 * to insert, and lookup.
 *
 * mildly formal-ish rules for fake memory:
 *  1. no duplicate entries.  
 *       if: 
 *           (<addr>, <val0>) \in memory
 *           (<addr>, <val1>) \in memory
 *      then <val0> == <val1>
 *  2. addresses are persistent: if you ever write <addr>,<val> to memory, 
 *     then <addr> stays there forever.
 *  3. values are persistent until the next write: if you ever write 
 *     (<addr>,<val> ) to memory, then until the next write, 
 *          if (<addr>, <v>) \in memory, then <v> == <val>.
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//#include "demand.h"
#include "rpi.h"
#include "pi-test.h"

/****************************************************************************
 *          DO NOT CHANGE THIS CODE (your changes go after it)
 *          DO NOT CHANGE THIS CODE (your changes go after it)
 *          DO NOT CHANGE THIS CODE (your changes go after it)
 *          DO NOT CHANGE THIS CODE (your changes go after it)
 *          DO NOT CHANGE THIS CODE (your changes go after it)
 */

typedef struct {
    const volatile void *addr;
    unsigned val;
} mem_t;

typedef struct {
    int op;
    mem_t v;
} log_ent_t;

static mem_t mk_mem(const volatile void *addr, unsigned val) {
    return (mem_t) { .addr = addr, .val = val };
}

// don't change routine so we can compare to everyone.
static void print_write(mem_t *m) {
    printf("\tTRACE:PUT32(%p)=0x%x\n", m->addr, m->val);
}
// don't change routine so we can compare to everyone.
static void print_read(mem_t *m) {
    printf("\tTRACE:GET32(%p)=0x%x\n", m->addr, m->val);
}



#define MAX_MEM 4096
static mem_t mem[MAX_MEM];
static int nmem;

// lookup <addr> in your memory implementation.  Returns the associated <mem_t>
// or 0 if there is none (null).
static mem_t* lookup(const volatile void *addr) {
    for(int i = 0; i < nmem; i++)
        if(mem[i].addr == addr)
            return &mem[i];
    return 0;
}

// insert (<addr>, <val>) into your fake memory.  
// do an assertion check that:
//   - before: <addr> is not in fake memory.
//   - after: <addr> is in fake memory and lookup returns the corect <val>
static mem_t *insert(const volatile void *addr, unsigned val) {
    assert(nmem < MAX_MEM);
    mem_t *m = &mem[nmem++];
    *m = mk_mem(addr, val);
    return m;
}

// return the value associated with <addr>.  if this <addr> has not been written
// to, insert a random() value and return that.
//
// before you return, call print_read on the mem_t associated with <addr>.
unsigned get32(const volatile void *addr) {
    mem_t *m = lookup(addr);
    if(!m)
        m = insert(addr,fake_random());
    print_read(m);
    return m->val;
}

// write (<addr>, <val>) into your fake memory.  if the <addr> is already in fake
// memory, overwrite it.
// 
// before you return, call print_write on the mem_t associated with <addr>.
void put32(volatile void *addr, unsigned val) {
    mem_t *m = lookup(addr);
    if(!m) {
        m = insert(addr,val);
    } else {
        m->addr = addr;
        m->val = val;
    }
    print_write(m);
}


