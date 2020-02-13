#ifndef __ESP_LEX_H__
#define __ESP_LEX_H__

#define LEX_SIMPLE  1
#define LEX_CIRC    2
#ifndef LEX_IMPL
// #   define LEX_IMPL LEX_SIMPLE
#   define LEX_IMPL LEX_CIRC
#endif

// #   define USE_IMPL LEX_CIRC
// #   define USE_LEX_SIMPLE

#if LEX_IMPL == LEX_CIRC
#   include "esp-lex-circ.h"
#elif LEX_IMPL == LEX_SIMPLE
#   include "esp-lex-simple.h"
#else
#   error "invalid define"
#endif

// initialize lexer --- client has to allocate.
void lex_init(lex_t *l, esp_handle_t h);

// read in more data.
int lex_readin(lex_t *l);

// read byte from <l>
int lex_getc(lex_t *l);

// non-destructively gets the next character in the input stream.
int lex_peek(lex_t *l);

// for debugging:
// print the entire stream of lexical data from the cursor <s> to the end.
void lex_dump(lex_t *s);
// as above, with no newline or preampble.
void lex_dump_str(lex_t *s);

#define lex_panic(_l, msg...) do {      \
    lex_dump(_l);                       \
    panic(msg);                         \
} while(0)

// should maybe have levels.
extern unsigned esp_verbose_p;
void esp_set_verbose(int verbose_p);

#define ESP_DEBUG
#ifdef ESP_DEBUG
#   define esp_debug(msg, args...)  do { if(esp_verbose_p) debug(msg, ##args); } while(0)
#else
#   define esp_debug(msg, args...) do { } while(0)
#endif

#endif
