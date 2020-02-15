#ifndef __ESP_LEX_SIMPLE_H__
#define __ESP_LEX_SIMPLE_H__
// engler, cs140e: using a ciruclar buffer is a pain for debugging.  this
// is a simpler one that buffers *all* input we have ever received so it's
// trivial to dump / debug / etc.

// huge buffer.
#define LEX_MAXBUF 1024 * 1024
typedef struct {
    unsigned pos;   // current position in the buffer.
    unsigned n;     // total characters in buffer
    uint8_t *stream;
    esp_handle_t h;
} lex_t;

static inline int lex_empty(lex_t *l) { 
    assert(l->pos <= l->n);
    return l->pos == l->n;
}

// current number of tokens in stream.
static inline int lex_nstream(lex_t *l) { 
    assert(l->pos <= l->n);
    return l->n - l->pos; 
}

static inline void lex_push_n(lex_t *l, uint8_t *buf, unsigned n) {
    assert(n>0);
    assert((l->n+n) < LEX_MAXBUF);

    memcpy(&l->stream[l->n], buf, n);
    l->n += n;
}

typedef struct {
    unsigned pos;
    lex_t *l;
} lex_ckpt_t;

// revert back to a checkpoint.
static inline void lex_restore_ckpt(lex_ckpt_t *ck) {
    ck->l->pos = ck->pos;
}

// returns a cursor to the current position in the stream.  used to 
// rewind after a failed match.
static inline lex_ckpt_t lex_get_ckpt(lex_t *l ){
    return (lex_ckpt_t) { .l = l, .pos = l->pos } ;
}

#define lex_dump_str lex_dump_str_simple
#define lex_getc lex_getc_simple
#define lex_peek lex_peek_simple
#define lex_init lex_init_simple

#endif
