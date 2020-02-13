#define LEX_IMPL LEX_SIMPLE
#include "libesp.h"

// read byte from <l>
int lex_getc_simple(lex_t *l) {
    if(lex_empty(l))
        lex_readin(l);
    assert(!lex_empty(l));
    return l->stream[l->pos++];
}

// non-destructively gets the next character in the input stream.
int lex_peek_simple(lex_t *l) {
    if(lex_empty(l))
        lex_readin(l);
    assert(!lex_empty(l));
    return l->stream[l->pos];
}

// used for debugging: print what we have.  helps figure out what when
// wrong by comparing to what we expected.
void lex_dump_str_simple(lex_t *l) {
    for(int i = l->pos; i < l->n; i++) {
        uint8_t c = l->stream[i];
        // in case people cut&paste output to esp-cat: don't have a space at
        // end of command!
        if(c == '\r')
            continue;
        // replace all non-printable with spaces other than newlines
        if(!isprint(c) && c != '\n')
            c = ' ';
        output("%c", c);
    }
}

// initialize lexer --- client has to allocate.
void lex_init_simple(lex_t *l, esp_handle_t h) {
    memset(l, 0, sizeof *l);
    l->stream = malloc(LEX_MAXBUF);
    assert(l->stream);

    l->h = h;
}
