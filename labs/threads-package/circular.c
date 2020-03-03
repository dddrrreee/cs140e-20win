// To test on linux:
//      gcc  -DLINUX -DLINUX_TEST circular.c
#ifdef LINUX
#   include <string.h>
#endif
#include "circular.h"

void cq_print(cq_t *c) {
    debug("cq print: head=%d, tail=%d, nelem=%d, empty=%d\n", 
                c->head, c->tail, cq_nelem(c), cq_empty(c));
}

void cq_ok(cq_t *c) {
    if(c->fence != 0x12345678)
        panic("fence is corrupted\n");
}

void cq_init(cq_t *c, unsigned errors_fatal_p) {
    memset(c, 0, sizeof *c);
    c->fence = 0x12345678;
    c->head = c->tail = 0;
    c->overflow = 0;
    c->errors_fatal_p = errors_fatal_p;
    assert(cq_empty(c));
    assert(!cq_full(c));
    assert(cq_nelem(c) == 0);
    cqe_t e = 0x12;
    assert(cq_pop_nonblock(c,&e) == 0 && e == 0x12);
}

int cq_pop_n_noblk(cq_t *q, void *data, unsigned n) {
    cq_ok(q);
    if(cq_nelem(q) < n)
        return 0;
    cq_pop_n(q,data,n);
    return 1;
}

// non-destructively peek ahead <n> entries.
int cq_peek_n(cq_t *c, cqe_t *v, unsigned n) {
    int sz = cq_nelem(c);
    if(n > sz)
        return 0;
    unsigned tail = c->tail;
    if(!cq_pop_n_noblk(c, v, n))
        return 0;
    c->tail = tail;
    assert(sz == cq_nelem(c));
    return 1;
}

// called from user space.
cqe_t cq_pop(cq_t *c) {
    cqe_t e;

	// wait til interrupt puts something here: if interrupts not enabled,
    // this will deadlock.
    while(!cq_pop_nonblock(c,&e))
        ;
        // panic("will deadlock: interrupts not enabled [FIXME]\n");
    return e;
}

void cq_pop_n(cq_t *c, void *data, unsigned n) {
    cqe_t *p = data;

    for(int i = 0; i < n; i++)
        p[i] = cq_pop(c);
}

int cq_push_n(cq_t *c, void *data, unsigned n) {
    cqe_t *p = data;

    if(cq_nspace(c) < n)
        return 0;
    for(int i = 0; i < n; i++)
        if(!cq_push(c, p[i]))
            panic("not handling this\n");
    if(n)
        assert(!cq_empty(c));
    return 1;
}

#ifdef LINUX_TEST
#include <string.h>

// is there a better test?  so sad can't just use klee...
int main() { 
    char buf[CQ_N], buf2[CQ_N];
	int i, ntests = 0, x;

    cq_t q;
    cq_init(&q,1);

    cq_push_n(&q, buf, 11);
    assert(cq_nelem(&q) == 11);
    assert(!cq_empty(&q));
    cq_pop_n(&q, buf, 11);
    assert(cq_nelem(&q) == 0);
    assert(cq_empty(&q));

    assert(cq_nelem(&q) == 0);
	for(i = 1; i < CQ_N; i++)  {
		cq_push(&q,i);
        assert(cq_nelem(&q) == i);
    }

	assert(cq_full(&q));
	for(i = 1; i < CQ_N; i++) {
        // fix this interface?
        cqe_t e;
        assert(cq_peek(&q, &e));
        assert(e == (cqe_t)i);
		assert(cq_pop(&q) == (cqe_t)i);
    }
	assert(cq_empty(&q));
    debug("nelem=%d, CQ_N=%d\n", cq_nelem(&q), CQ_N);
	for(i = 0; i < CQ_N; i++) 
        buf[i] = i;
    cq_push_n(&q, buf, CQ_N-1);
    assert(!cq_empty(&q));
    debug("nelem=%d, CQ_N=%d\n", cq_nelem(&q), CQ_N);

    assert(cq_nelem(&q) == CQ_N-1);

    cq_pop_n(&q, buf2, CQ_N-1);
    assert(memcmp(buf, buf2, CQ_N-1) == 0);

	assert(cq_empty(&q));
	for(i = 1; i < CQ_N; i++) 
		cq_push(&q,i);
    

	int expected = 1;
	for(; i < CQ_N*1000; i++) {
		assert(cq_full(&q));
		x = cq_pop(&q);
		assert(!cq_empty(&q));
		assert((cqe_t)x == (cqe_t)expected);
		expected++;
		cq_push(&q,i);
		ntests++;
	}
	printf("tests=%d\n", ntests);
    printf("SUCCESS!\n");
	return 0;
}
#endif
