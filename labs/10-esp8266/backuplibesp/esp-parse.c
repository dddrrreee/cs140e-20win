/*
 * engler, cs140e: simplistic parsing code for the esp8266.  i'm trying to
 * do this on the quick/dirty vs using a parser-generator since the input 
 * format is pretty simple and this is an OS class so alot of you haven't
 *
 * the two annoying complications:
 *  1. the characters from the esp can trickle in at an arbitrary rate --- so we 
 *      can't assume we can get everything with a single read call. 
 *  2. we don't always know what we are matching against, so need a way to undo
 *     the input consumed from a failed match..
 * this is why we use the lexing code (esp-lex.c).  it stores characters in a circular
 * buffer as it receives them; if a match needs more iput, it calls a client supplised
 * routine to grab more.
 */
// i *think* stdarg.h is safe to include on the pi.
#include <stdarg.h>

#define USE_LEX_CIRC
#include "libesp.h"


/*************************************************************
 * trivial (non-destructive) parsing helpers that do not consume
 * input.
 */

// we could get rid of most of this if our gettok skipped sapces.
static int is_eol_ch(int ch) {
    return ch == '\r' || ch == '\n';
}
static int is_eol(lex_t *l) {
    return is_eol_ch(lex_peek(l));
}

// we have a limited view of spaces: otherwise
// will wander onto the next line.
static int is_space_ch(int ch) {
    return ch == ' ' || ch == '\t';
}
static int is_space(lex_t *l) {
    return is_space_ch(lex_peek(l));
}
static void skip_blank_space(lex_t *l) {
    while(is_space(l))
        lex_getc(l);
}
// skip spaces?  sometimes we get a \r\n sometimes a \n\r
static int match_eol(lex_t *l) {
    lex_ckpt_t ck = lex_get_ckpt(l);
    skip_blank_space(l);
    if(!is_eol(l)) {
        lex_restore_ckpt(&ck);
        return 0;
    }
    lex_getc(l);

    // consume as many eol as possible without
    // reading using lex_readin.
    while(lex_nstream(l) && is_eol(l))
        lex_getc(l);
    return 1;
}
// if not blank, removes no characters.
int match_blank_line(lex_t *l) {
    lex_ckpt_t ck = lex_get_ckpt(l);
    skip_blank_space(l);
    if(match_eol(l))
        return 1;
    lex_restore_ckpt(&ck);
    return 0;
}
static void skip_blank_lines(lex_t *l) {
    while(match_blank_line(l))
        ;
}

// read the entire line into <buf> and null terminate it.
// returns 0 if it was empty.
int get_nonblank_line(lex_t *l, char *buf, unsigned nbytes) {
    if(match_blank_line(l))
        return 0;
    skip_blank_space(l);
    // get the raw line.
    int i;
    for(i = 0; i < nbytes; i++) {
        if(match_eol(l))
            break;
        *buf++ = lex_getc(l);
    }
    if(i == nbytes)
        lex_panic(l, "buffer too small nybtes=%d\n", nbytes);
    *buf++ = 0;
    return i;
}

// right now eats the spaces on mismatch.  idk if matters.
int match_uint(lex_t *l, unsigned *x) {
    skip_blank_space(l);

    // we are not handling +/-.
    unsigned i;
    for(*x = i = 0; isdigit(lex_peek(l)); i++)
        *x = *x*10 + lex_getc(l) - '0';

    if(i) 
        esp_debug("matched uint=%u, i=%d\n", *x, i);
    return i != 0;
}

// if first character is not \", then return 0, otherwise
// destructively match string.  panic if no matching \".
int match_literal_string(lex_t *l, char *out, unsigned nbytes) {
    skip_blank_space(l);
    if(lex_peek(l) != '\"')
        return 0;
    lex_getc(l);

    // make sure we don't run off the end.
    for(int i = 0; i < nbytes; i++) {
        if((*out++ = lex_getc(l)) == '\"') {
            out[-1] = 0;
            return 1;
        }
    }
    lex_panic(l, "malformed string, exceeded size: <%s>\n", out);
}

// panic if <l> does not contain a string as next match
void expect_literal_string(lex_t *l, char *out, unsigned nbytes) {
    if(!match_literal_string(l,out,nbytes))
        lex_panic(l, "expected a string!\n");
}
// panic if <l> does not contain a uint as next match
void expect_uint(lex_t *l, unsigned *x) {
    if(!match_uint(l,x))
        lex_panic(l, "expected uint, have something else\n");
}
// panic if <l> does not contain a blank line.
void expect_blank_line(lex_t *l) {
    if(!match_blank_line(l))
        lex_panic(l,"expected blank line!\n");
}
// panic if <l> does not contain <tok> as next match
void expect_tok(lex_t *l, const char *tok) {
    if(!match_tok(l, tok))
        lex_panic(l, "expected token=<%s>, received other", tok);
}

/*************************************************************
 * these are the two main-used parsing routines.
 */

// slightly different semantics: we match prefix (other than blank spaces),
// but reject if it is a substring of a longer token. for example:
//  "var" would not match "variable", but it would match "var+"
//
// right now is still sensitive to garbage (e.g., a bunch of stuff that 
// has the keyword embedded in it).
int match_tok(lex_t *l, const char *tok) {
    lex_ckpt_t ckpt = lex_get_ckpt(l);
    skip_blank_lines(l);
    skip_blank_space(l);

    const char *p = tok;
    assert(*p);
    while(*p) {
        if(*p++ != lex_getc(l))
            goto fail;
    }
    // reject a substring match --- i think any other character is ok? 
    if(isalpha(p[-1]) && isalpha(lex_peek(l)))
        goto fail;
    return 1;

fail:
    lex_restore_ckpt(&ckpt);
    return 0;
}

// semantics: match a single line of text: ends with a \r\n
int matchv(lex_t *l, const char *fmt, va_list args) {
    lex_ckpt_t ckpt = lex_get_ckpt(l);


    const char *fmt_string = fmt;
    int must_match = 0;
    skip_blank_lines(l);
    skip_blank_space(l);

    for(; *fmt; fmt++) {
        assert(!is_eol_ch(*fmt));
        if(is_eol_ch(lex_peek(l)))
            goto fail;

        // ignore spaces in both the fmt and esp input.  idk if this is best.
        if(*fmt != '%') {
            if(*fmt != ' ') {
                skip_blank_space(l);
                if(lex_getc(l) != *fmt)
                    goto fail;
            }
        } else {
            // after the first entire literal substring matches, we assume the 
            // rest *must match.
            // this check: we hit a % and it's after the first character, so we've
            // satisfied the substring match criteria
            if(fmt != fmt_string)
                must_match = 1;

            fmt++;
            switch(*fmt) { 
                // %s = literal string with quotes.
                case 's':  {
                    // ugh: how to match this?
                    char *s = va_arg(args, char *);
                    // unsigned nbytes = va_arg(args, unsigned);
                    if(!match_literal_string(l, s,32))
                        goto fail;
                    break;
                }
                case 'u': {
                    unsigned *u = va_arg(args, unsigned *);
                    if(!match_uint(l,u))
                        goto fail;
                    break;
                }
                default: panic("illegal format string: <%s>\n", fmt_string);
            }
        }
    }
    return 1;
    
fail:
    if(must_match) 
        lex_panic(l, "must match failed: malformed value for <%s>!\n", fmt_string);
    lex_restore_ckpt(&ckpt);
    return 0;
}

int match_linev(lex_t *l, const char *fmt, va_list args) {
    int res = matchv(l, fmt, args);
    if(!res)
        return res;
    esp_debug("matched line <%s>!\n", fmt);

    // we expect an EOL: maybe should change so doesn't die, like the
    // other matches.
    if(!match_eol(l))
        lex_panic(l, "did not match end of line\n");
    return res;
}

// semantics: match a single line of text: ends with a \r\n
int match_line(lex_t *l, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int res = match_linev(l, fmt, args);
    va_end(args);
    return res;
}

// semantics: match partial line: skip initial spaces.
int match(lex_t *l, const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    int res = matchv(l, fmt, args);
    va_end(args);
    if(!res)
        return res;
    esp_debug("matched substring <%s>!\n", fmt);
    return res;
}
