// encode/decode utf8-unicode.  
// from:
//      https://rosettacode.org/wiki/UTF-8_encode_and_decode#C

//#define TEST
#ifdef TEST
#   include <assert.h>
#   include <stdio.h>
#else
#   include "rpi.h"
#   define printf printk
#endif

#include <inttypes.h>
#include "unicode-utf8.h"

typedef struct {
    char mask;    /* char data will be bitwise AND with this */
    char lead;    /* start bytes of current char in utf-8 encoded character */
    uint32_t beg; /* beginning of codepoint range */
    uint32_t end; /* end of codepoint range */
    int bits_stored; /* the number of bits from the codepoint that fits in char */
}utf_t;

static utf_t * utf[] = {
    /*             mask        lead        beg      end       bits */
    [0] = &(utf_t){0b00111111, 0b10000000, 0,       0,        6    },
    [1] = &(utf_t){0b01111111, 0b00000000, 0000,    0177,     7    },
    [2] = &(utf_t){0b00011111, 0b11000000, 0200,    03777,    5    },
    [3] = &(utf_t){0b00001111, 0b11100000, 04000,   0177777,  4    },
    [4] = &(utf_t){0b00000111, 0b11110000, 0200000, 04177777, 3    },
          &(utf_t){0},
};


int codepoint_len(const uint32_t cp) {
    int len = 0;
    for(utf_t **u = utf; *u; ++u) {
        if((cp >= (*u)->beg) && (cp <= (*u)->end))
            break;
        ++len;
    }
    if(len > 4) /* Out of bounds */
        return -1;
    return len;
}

int utf8_len(const char ch) {
    int len = 0;
    for(utf_t **u = utf; *u; ++u) {
        if((ch & ~(*u)->mask) == (*u)->lead)
            break;
        ++len;
    }
    if(len > 4) /* Malformed leading byte */
        return -1;
    return len;
}

char *to_utf8(const uint32_t cp)
{
    static char ret[5];
    const int bytes = codepoint_len(cp);

    if(bytes < 0)
        return 0;
    int shift = utf[0]->bits_stored * (bytes - 1);
    ret[0] = (cp >> shift & utf[bytes]->mask) | utf[bytes]->lead;
    shift -= utf[0]->bits_stored;
    for(int i = 1; i < bytes; ++i) {
        ret[i] = (cp >> shift & utf[0]->mask) | utf[0]->lead;
        shift -= utf[0]->bits_stored;
    }
    ret[bytes] = '\0';
    return ret;
}

uint32_t to_cp(const char chr[4]) {
    int bytes = utf8_len(*chr);
    if(bytes < 0)
        return 0;
    int shift = utf[0]->bits_stored * (bytes - 1);
    uint32_t codep = (*chr++ & utf[bytes]->mask) << shift;

    for(int i = 1; i < bytes; ++i, ++chr) {
        shift -= utf[0]->bits_stored;
        codep |= ((char)*chr & utf[0]->mask) << shift;
    }
    return codep;
}

/*
    Character  Unicode  UTF-8 encoding (hex)
    ----------------------------------------
    A          U+0041   41
    ö          U+00f6   c3 b6
    Ж          U+0416   d0 96
    €          U+20ac   e2 82 ac
    𝄞          U+1d11e  f0 9d 84 9e
 */
int test_main(void) {
    const uint32_t *in, input[] = {0x0041, 0x00f6, 0x0416, 0x20ac, 0x1d11e, 'f', 'G', 0x0};

    printf("Character  Unicode  UTF-8 encoding (hex)\n");
    printf("----------------------------------------\n");
    for(in = input; *in; ++in) {
        char *utf8 = to_utf8(*in);
        assert(utf8);
        uint32_t codepoint = to_cp(utf8);

        printf("%s          U+%x", utf8, codepoint);
        for(int i = 0; utf8[i] && i < 4; ++i)
            printf("%x ", utf8[i]);
        printf("\n");
    }
    return 0;
}

#ifdef TEST
int main(void) {
    const uint32_t *in, input[] = {0x0041, 0x00f6, 0x0416, 0x20ac, 0x1d11e, 'f', 'G', 0x0};

    printf("Character  Unicode  UTF-8 encoding (hex)\n");
    printf("----------------------------------------\n");
    for(in = input; *in; ++in) {
        char *utf8 = to_utf8(*in);
        assert(utf8);
        uint32_t codepoint = to_cp(utf8);

        printf("%s          U+%-7.4x", utf8, codepoint);
        for(int i = 0; utf8[i] && i < 4; ++i)
            printf("%hhx ", utf8[i]);
        printf("\n");
    }
    return 0;
}
#endif
