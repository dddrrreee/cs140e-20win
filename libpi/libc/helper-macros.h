// helper functions to deal with bitfields and sending structures to PUT/GET
#ifndef __RPI_MACROS_H__
#define __RPI_MACROS_H__

// i think this only works for power of 2?
#define is_aligned(x, a)        (((x) & ((typeof(x))(a) - 1)) == 0)
#define is_aligned_ptr(x, a)        is_aligned((uint32_t)x,a)

#define pi_roundup(x,n) (((x)+((n)-1))&(~((n)-1)))

// check bitfield positions.
#define check_bitfield(T, field, off, nbits) do {                       \
    union _u {                                                      \
        T s;                                                    \
        unsigned u;                                             \
    } x = (union _u) { .s = (T){ .field = 1 } };                    \
    unsigned exp = 1<<off;                                          \
    if(x.u != exp)                                                  \
        panic("offset wrong: expect %x got %x %s [off=%d,nbits=%d]\n", \
                        exp,x.u,#field,off,nbits);        \
    assert(x.u == exp);                                             \
                                                                        \
    unsigned shift = 32 - nbits;                                \
    unsigned set_all = (~0UL << shift) >> shift;                      \
    exp = set_all << off;                                           \
    x.s.field = ~0;                                                 \
    if(x.u != exp)                                                  \
        panic("width wrong: expect %x got %x %s [off=%d,nbits=%d]\n", \
                    exp,x.u,#field,off,nbits);        \
    assert(x.u == exp);                                             \
} while(0)

#define print_field(x, field) do {                   \
    printk("\t0b%b\t= %s\n", (x)->field, #field);   \
    if(((x)->field) > 8)                            \
        printk("\t0x%x\n", (x)->field);             \
} while(0)


#if 0
// check bitfield positions.
#define check_off_static(T, field, off, nbytes) do {        \
    AssertNow(offsetof(T, field) == off);                   \
    AssertNow(sizeof ((T *)0)->field == nbytes);            \
} while(0)
#endif

#define check_off_static(T, field, off, nbytes)                           \
    _Static_assert(offsetof(T, field) == off, "field offset is wrong");   \
    _Static_assert(sizeof ((T *)0)->field == nbytes, "nbytes is wrong")


#define check_off(T, field, off, nbytes) do {                           \
    unsigned got = offsetof(T, field);                              \
    if(got != off)                                                  \
        panic("field <%s> offset wrong: expect %d got %d\n",    \
                                          #field, off, got);            \
    unsigned sz = sizeof ((T *)0)->field;                           \
    if(sz != nbytes)                                                \
        panic("field <%s> wrong size: expect %d, got %d\n",     \
                                          #field, nbytes, sz);          \
} while(0)


// pi-specific implementations of common routines.

#define aligned(ptr, n)  ((unsigned)ptr % n == 0)
#define aligned4(ptr)  aligned(ptr,4)

#define zero(x) memset(x, 0, sizeof *x)

// type pun a uint32 into a type T (hopefully) without breaking gcc aliasing
// assumptions.
#define u32_to_T(_T, _val) ({                                           \
        AssertNow(sizeof(_T) == 4);                                     \
        AssertNow(sizeof(_val) == 4);                                   \
        union _u { _T v; unsigned u; } u = (union _u){ .u = _val };     \
        u.v;                                                            \
})

// type pun a type T into a uint32 (hopefully) without breaking gcc aliasing
// assumptions.
#define T_to_u32(_T, _val) ({                                           \
        AssertNow(sizeof(_T) == 4);                                     \
        AssertNow(sizeof(_val) == 4);                                   \
        union _u { _T vv; uint32_t uu; } u = (union _u){ .vv = _val };     \
        u.uu;                                                            \
})


// ugh.  gcc struct packing is dangerous, so byte structs will actually
// be 4 bytes b/c of alignment.    it's up to the user to make sure that
// the byte struct is the first byte of struct!
#define u8_to_T(_T, _val) ({                                           	\
	_T _to;								\
	memset(&_to, 0, sizeof _to); \
	uint8_t _from = _val;						\
	memcpy(&_to, &_from, 1);					\
        _to;                                                           	\
})

#define T_to_u8(_T, _val) ({                                           	\
	uint8_t _to;							\
	_T _from = _val;						\
	memcpy(&_to, &_from, 1);					\
        _to;                                                           	\
})

/*
 * put32_T/get32_T force storing/loading of a 32-bit structure using a 
 * a single 32-bit store instruction.
 * We do it this way b/c it's unclear if storing a 32-bit hw register a 
 * byte at a time gives ok values and since we cannot guarantee compiler 
 * doesn't do this, we force single-store/load behavior by calling asm.
 *
 * Our general device strategy:
 *	- if register is a single field, even < 32, just write the unsigned
 *	directly b/c that will also write 0s.
 *	- if register has multiple subfields and reserved bits, implement
 *	"clear" routines that read the hw value, clear the reserved 
 *	bits and return the result.
 */

// do a put32 of a structure of type(_s)
#define put32_T(_s, _v) put32(&_s, T_to_u32(typeof(_v), _v))

// same but takes a raw int address.
#define PUT32_T(_s, _v) PUT32(_s, T_to_u32(typeof(_v), _v))

// do a get32 into a structure of type(*_x)
#define get32_T(_x) u32_to_T(typeof(*_x), get32(_x))

// <lhs> <---- get32(<rhs>): use if <rhs> has the wrong type.
#define assign32_T(_lhs, _rhs) ((_lhs) = u32_to_T(typeof(_lhs), GET32(_rhs)))


#if 0
// example usage
typedef struct fake_dev {
    uint32_t a;
    uint32_t b;
} fake_dev_t;

#include "libc/helper-macros.h"
void check_dev_offsets(void) {
    check_off_static(fake_dev_t, a, 0, 4);
    check_off_static(fake_dev_t, b, 7, 4);
}
#endif

#endif


