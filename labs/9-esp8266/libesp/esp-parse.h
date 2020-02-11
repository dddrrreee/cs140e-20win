#ifndef __ESP_PARSE_H__
#define __ESP_PARSE_H__
#include "esp-lex.h"

// panic if <l> is not a blank line.
void expect_blank_line(lex_t *l);

// read the entire line into <buf> and null terminate it.
// returns 0 if it was a blank line.
int get_nonblank_line(lex_t *l, char *buf, unsigned nbytes);

/*
 * match token: does the next token in <l> match <tok>?  
 *  - does not consume input on failed matches.
 * - ignores blank spaces in the input <l>
 */
int match_tok(lex_t *l, const char *tok);


/*
 * semantics: match a single line of text: ends with a \r\n
 * - does not consume input on failed match.
 * - ignores blank spaces in both the <l> input anf <fmt>
 *
 * note: once we have matched "sufficiently enough" of the input,
 * it will panic on a mismatch.  right now this is: you matched
 * everything up until the first "%" in the format string.
 */
int match_line(lex_t *l, const char *fmt, ...);

// if not blank, removes no characters.
int is_blank_line(lex_t *l);

#endif
