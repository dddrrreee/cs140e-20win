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
#include "libesp.h"

int exp_tok(lex_t *l, int exp) {
    int ch;
    // this is too 
    if((ch = lex_getc(l)) == exp)
        return 1;
    // esp_log("expected %d have %d\n", exp, ch);
    lex_panic(l, "expect: waiting for <%d> have <%d>\n", exp,ch);
}

/*************************************************************
 * trivial (non-destructive) parsing helpers that do not consume
 * input.
 */

static int is_eol_ch(int ch) {
    return ch == '\r' || ch == '\n';
}
static int is_eol(lex_t *l) {
    return is_eol_ch(lex_peek(l));
}

// we have a limited view of spaces: otherwise
// will wander onto the next line.
static int is_space(lex_t *l) {
    return lex_peek(l) == ' ' || lex_peek(l) == '\t';
}
static void skip_blank_space(lex_t *l) {
    while(is_space(l))
        lex_getc(l);
}

// skip spaces?  sometimes we get a \r\n sometimes a \n\r
static int match_eol(lex_t *l) {
#if 0
    int eol_p = 0;
    while(is_eol_ch(lex_peek(l))) {
        lex_getc(l);
        eol_p = 1;
    }
    return eol_p;
#else
    if(!is_eol(l))
        return 0;
    lex_getc(l);

    // consume as many eol as possible without
    // reading using lex_readin.
    while(lex_nstream(l) && is_eol(l))
        lex_getc(l);

    return 1;
#if 0
    ch = lex_peek(l);
    if(!is_eol_ch(ch))
        lex_pnic(l, "expected a newline or cr, have %d\n", ch);
    return 1;

    if(lex_peek(l) == '\r') {
        lex_getc(l);
        exp_tok(l, '\n');
        return 1;
    }
#endif
#endif
    return 0;
}

static int match_blank_line(lex_t *l) {
    skip_blank_space(l);
    return match_eol(l);
}

// if not blank, removes no characters.
int is_blank_line(lex_t *l) {
    lex_ckpt_t ck = lex_get_ckpt(l);
    if(match_blank_line(l)) 
        return 1;
    lex_restore_ckpt(&ck);
    return 0;
}


// right now eats the spaces on mismatch.  idk if matters.
static int match_uint(lex_t *l, unsigned *x) {
    skip_blank_space(l);

    // we are not handling +/-.
    unsigned i;
    for(*x = i = 0; isdigit(lex_peek(l)); i++)
        *x = *x*10 + cq_pop(&l->stream) - '0';

    if(i) esp_debug("matched uint=%u, i=%d\n", *x, i);
    return i != 0;
}

// if first character is not \", then return 0, otherwise
// destructively match string.  panic if no matching \".
int match_string(lex_t *l, char *out, unsigned nbytes) {
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
static void expect_string(lex_t *l, char *out, unsigned nbytes) {
    if(!match_string(l,out,nbytes))
        lex_panic(l, "expected a string!\n");
}
// panic if <l> does not contain a uint as next match
static int expect_uint(lex_t *l, unsigned *x) {
    if(!match_uint(l,x))
        lex_panic(l, "expected uint, have something else\n");
    return 1;
}

// panic if <l> does not contain a blank line.
void expect_blank_line(lex_t *l) {
    skip_blank_space(l);
    if(!match_eol(l)) 
        lex_panic(l,"expected blank line!\n");
}

// panic if <l> does not contain <tok> as next match
static int expect(lex_t *l, char *tok) {
    if(!match_tok(l, tok))
        lex_panic(l, "expected token=<%s>, received other", tok);
    return 1;
}

static void skip_blank_lines(lex_t *l) {
    while(1) {
        lex_ckpt_t ck = lex_get_ckpt(l);
        if(!match_blank_line(l)) {
            lex_restore_ckpt(&ck);
            return;
        }
    }
}

// read the entire line into <buf> and null terminate it.
// returns 0 if it was empty.
int get_nonblank_line(lex_t *l, char *buf, unsigned nbytes) {
    // assume empty.
    int non_empty_p = 0;

    // get the raw line.
    int i;
    for(i = 0; i < nbytes; i++) {
        if(match_eol(l))
            break;
        // non-space character = not empty.
        if(!is_space(l))
            non_empty_p = 1;
        *buf++ = lex_getc(l);
    }
    if(i == nbytes)
        lex_panic(l, "buffer too small nybtes=%d\n", nbytes);
    *buf++ = 0;
    return non_empty_p;
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
    // this will send all the way back.
    lex_restore_ckpt(&ckpt);
    return 0;
}

// semantics: match a single line of text: ends with a \r\n
int match_line(lex_t *l, const char *fmt, ...) {
    const char *fmt_string = fmt;
    lex_ckpt_t ckpt = lex_get_ckpt(l);
    int must_match = 0;

    skip_blank_lines(l);

    va_list args;
    va_start(args, fmt);
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
                    char *s = va_arg(args, char *);
                    unsigned nbytes = va_arg(args, unsigned);
                    if(!match_string(l, s,nbytes))
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
    esp_debug("matched <%s>!\n", fmt_string);
    // must match end.
    if(!match_eol(l))
        lex_panic(l, "did not match end of line\n");

    va_end(args);
    return 1;
    
fail:
    lex_restore_ckpt(&ckpt);
    if(must_match) 
        lex_panic(l, "must match failed: malformed value for <%s>!\n", fmt_string);
    va_end(args);
    return 0;
}
