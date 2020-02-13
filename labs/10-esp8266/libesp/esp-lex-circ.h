#ifndef __ESP_LEX_CIRC_H__
#define __ESP_LEX_CIRC_H__

// #define CQ_N (8192 * 2)
#define CQ_N (1024)
#include "../libpi/libc/circular.h"
typedef struct {
    unsigned tail;  // the initial tail.
    unsigned n;     // how many characters we moved.   this can't be more than N
    cq_t stream;
    esp_handle_t h;
} lex_t;

#define lex_empty(l) cq_empty(&(l)->stream)
// current number of tokens in stream.
#define lex_nstream(l) cq_nelem(&(l)->stream)

typedef struct {
    unsigned tail;  // the initial tail.
    unsigned n;     // the number of characters in buffer at ckpt
    lex_t *l;
} lex_ckpt_t;

// revert back to a checkpoint.
static inline void lex_restore_ckpt(lex_ckpt_t *ck) {
    ck->l->stream.tail = ck->tail;
}
// returns a cursor to the current position in the stream.  used to 
// rewind after a failed match.
static inline lex_ckpt_t lex_get_ckpt(lex_t *l ){
    return (lex_ckpt_t) { .l = l, .tail = l->stream.tail } ;
}
static inline void lex_push_n(lex_t *l, uint8_t *buf, unsigned n) {
    if(!cq_push_n(&l->stream, buf, n))
        panic("push failed!\n");
}

#define lex_getc lex_getc_circ
#define lex_peek lex_peek_circ
#define lex_dump_str lex_dump_str_circ
#define lex_init lex_init_circ

#endif
