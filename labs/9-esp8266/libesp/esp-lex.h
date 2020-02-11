#ifndef __ESP_LEX_H__
#define __ESP_LEX_H__

#define CQ_N (8192 * 2)
#include "../libpi/libc/circular.h"
typedef struct {
    unsigned tail;  // the initial tail.
    unsigned n;     // how many characters we moved.   this can't be more than N
    cq_t stream;
    esp_handle_t h;
} lex_t;

#define lex_empty_stream(l) cq_empty((l)->stream)
// current number of tokens in stream.
#define lex_nstream(l) cq_nelem(&(l)->stream)

typedef struct {
    unsigned tail;  // the initial tail.
    unsigned n;     // the number of characters in buffer at ckpt
    lex_t *l;
} lex_ckpt_t;

// create a buffered lexer.
void lex_init(lex_t *l, esp_handle_t h);

// take a snapshot that you can roll back.
lex_ckpt_t lex_get_ckpt(lex_t *l);
// revert back to a checkpoint.
void lex_restore_ckpt(lex_ckpt_t *ck);

// for debugging:
// print the entire stream of lexical data from the cursor <s> to the end.
void lex_dump(lex_t *s);
// as above, with no newline or preampble.
void lex_dump_str(lex_t *s);
// void lex_dump_ck(lex_ckpt_t *s);

#define lex_panic(_l, msg...) do {      \
    debug(msg);                       \
    lex_dump(_l);                       \
    panic("FATAL ERROR\n");             \
} while(0)

#define ESP_DEBUG
#ifdef ESP_DEBUG
#   define esp_debug   debug
#else
#   define esp_debug(msg, args...) do { } while(0)
#endif


// read byte from <l>
int lex_getc(lex_t *l);

// non-destructively gets the next character in the input stream.
int lex_peek(lex_t *l);

#endif
