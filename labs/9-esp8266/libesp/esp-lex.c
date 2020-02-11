/*
 * engler, cs140e: low-level routines to support lexing.
 *
 * mainly, input, output, checkpoint/restore:
 *  1. code to read into circular buffer (lex_readin)
 *  2. get a character (lex_getc)
 *  3. take and restore checkpoints in input stream (lex_get_ckpt, lex_restore_ckpt)
 *  4. basic debugging by dumping all input (lex_dump).
 *
 * hopefully you should not have to modify this.  please let me know if 
 * broken.
 */
#include "libesp.h"

// initialize lexer --- client has to allocate.
void lex_init(lex_t *l, esp_handle_t h) {
    memset(l, 0, sizeof *l);
    cq_init(&l->stream,1);
    l->tail = l->stream.tail;
    l->n = 0;
    l->h = h;
}

// can reset the stream to the snapshot in <l>
void lex_restore_ckpt(lex_ckpt_t *ck) {
    ck->l->stream.tail = ck->tail;
}

// returns a cursor to the current position in the stream.  used to 
// rewind after a failed match.
lex_ckpt_t lex_get_ckpt(lex_t *l ){
    return (lex_ckpt_t) { .l = l, .tail = l->stream.tail } ;
}

// read in more data.
int lex_readin(lex_t *l) {
    assert(cq_empty(&l->stream));
    assert(cq_nelem(&l->stream) == 0);

    // i think to get around hold and wait, have to allow timeout.
    for(int i = 0; !esp_has_data_timeout(l, 1000*10); i++)  {
        if(i % 100  == 0)
            output("%d: waiting on input from esp\n", i);
    }

    unsigned nbytes = 0; 
    do { 
        char buf[1024];
        int n;

        // returns number of bytes or -1.
        if((n = esp_read(l, buf, sizeof buf, 0)) <= 0)
            panic("server_read failed: %d\n", n);
        cq_push_n(&l->stream, buf, n);
        nbytes += n;
        assert(cq_nelem(&l->stream) == nbytes);
    } while(esp_has_data_timeout(l, 0));

    return nbytes;
}

// used for debugging: print what we have.  helps figure out what when
// wrong by comparing to what we expected.
void lex_dump_str(lex_t *l) {
    cq_t *q = &l->stream;

    // we do not modify tail b/c we do not want any interrupt handling
    // putc's to overwrite data in case the client wants to continue
    // getting stuff.
    for(unsigned i = q->tail; i != q->head; i = (i+1) % CQ_N)
        output("%c", q->c_buf[i]);
}
void lex_dump(lex_t *l) {
    output("all unconsumed input currently buffered from ESP:<\n");
    lex_dump_str(l);
    output(">\n");
}

int lex_has_data(lex_t *l) {
    return !cq_empty(&l->stream) || esp_has_data_timeout(l, 0);
}

// read byte from <l>
int lex_getc(lex_t *l) {
    l->n++;
#if 0
    // this is b/c we cannot roll back if we read too much.
    if(l->n > CQ_N)
        lex_panic(l, "read in too many characters since the checkpoint: %d\n", l->n);
#endif

    if(cq_empty(&l->stream))
        lex_readin(l);
    assert(!cq_empty(&l->stream));
    return cq_pop(&l->stream);
}

// non-destructively gets the next character in the input stream.
int lex_peek(lex_t *l) {
    if(cq_empty(&l->stream))
        lex_readin(l);
    assert(!cq_empty(&l->stream));

    uint8_t uc = 0;
    if(!cq_peek(&l->stream, &uc))
        panic("impossible");
    return uc;
}
