#define LEX_IMPL LEX_CIRC
#include "libesp.h"

// read byte from <l>
int lex_getc_circ(lex_t *l) {
    l->n++;
    if(lex_empty(l))
        lex_readin(l);
    assert(!cq_empty(&l->stream));
    return cq_pop(&l->stream);
}

// non-destructively gets the next character in the input stream.
int lex_peek_circ(lex_t *l) {
    if(cq_empty(&l->stream))
        lex_readin(l);
    assert(!cq_empty(&l->stream));

    uint8_t uc = 0;
    if(!cq_peek(&l->stream, &uc))
        panic("impossible");
    return uc;
}

// used for debugging: print what we have.  helps figure out what when
// wrong by comparing to what we expected.
void lex_dump_str_circ(lex_t *l) {
    cq_t *q = &l->stream;

    // we do not modify tail b/c we do not want any interrupt handling
    // putc's to overwrite data in case the client wants to continue
    // getting stuff.
    for(unsigned i = q->tail; i != q->head; i = (i+1) % CQ_N)
        output("%c", q->c_buf[i]);
}

// initialize lexer --- client has to allocate.
void lex_init_circ(lex_t *l, esp_handle_t h) {
    memset(l, 0, sizeof *l);
    cq_init(&l->stream,1);
    l->tail = l->stream.tail;
    l->n = 0;
    l->h = h;
}
