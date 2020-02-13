#ifndef __ESP_PARSE_H__
#define __ESP_PARSE_H__
#include <stdarg.h>  // hopefully is ok on the pi.
#include "esp-lex.h"
/*
 * engler, cs140e: parsing code.   
 * the general rule: 
 *  - <match_*> : these routines non-destructively check
 *    a predicate, with the exception that they consume
 *     whitespace.    
 *  - <expect_*> panic's if the predicate does not match.
 */

/*
 * match token: does the next token in <l> match <tok>?  
 *  - does not consume input on failed matches.
 * - ignores blank spaces in the input <l>
 */
int match_tok(lex_t *l, const char *tok);
void expect_tok(lex_t *l, const char *tok);

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
// same: takes a va pointer.
int match_linev(lex_t *l, const char *fmt, va_list args);

// semantics: match partial line: skip initial spaces.  no eol necessary.
int match(lex_t *l, const char *fmt, ...);

// internal match routine, takes <va_list> versus format
// string.
int matchv(lex_t *l, const char *fmt, va_list args);

// read the entire line into <buf> and null terminate it.
// returns 0 if it was a blank line.
int get_nonblank_line(lex_t *l, char *buf, unsigned nbytes);

// returns 1 if matched unsigned, 0 otherwise.  nondestructive.
int match_uint(lex_t *l, unsigned *x);
void expect_uint(lex_t *l, unsigned *x);

// match a literal string with \" quotes: e.g., "TCP".  
// on match, consumes the string from <l> and returns 1.
// otherwise returns 0 and does not consume string.
int match_literal_string(lex_t *l, char *out, unsigned nbytes);
void expect_literal_string(lex_t *l, char *out, unsigned nbytes);

// if not blank, removes no characters.
int match_blank_line(lex_t *l);
void expect_blank_line(lex_t *l);

#endif
