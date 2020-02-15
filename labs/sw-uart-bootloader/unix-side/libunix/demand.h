#ifndef __DEMAND_H__
#define __DEMAND_H__
// engler, cs140e.
// various helpful error-checking / debugging macros.
//
// You should work through and figure out what the different operators
// do.  E.g.,:
//  - gcc's variadic macros.
//  - using macros so you can print the file, function, linenumber 
//    that a usage occurs.
//  - the use of "do { ... } while()" to guard against possilbe problems
//    with macro-expansion.
//
// These techniques can 1/ clean up your code and 2/ make it easy
// to change the functionality of your code without modifications.
// 
// Of course, macro's are easily evil and gross (perhaps even these!),
// so caveat emptor.
//
// TODO: modify so that you can compile with -DNDEBUG and have
// the demand / debug calls safely compiled out.
#include <stdio.h>
#include <stdlib.h>

// use to "stringize" a macro argument.
#define _XSTRING(x) #x

// XXX: need to change these to not use <debug>: want to be able to compile it out.

// stolen from van wyk.
// usage: 
//	demand(expr, msg)
// prints <msg> if <expr> is false.
// example:
//	demand(x < 3, x has bogus value!);
// note: 
//	if <_msg> contains a ',' you'll have to put it in quotes.
#define demand(_expr, _msg, args...) do {			                \
    if(!(_expr)) { 							                        \
        fprintf(stderr, "%s:%s:%d: Assertion '<%s>' failed:",       \
                        __FILE__, __FUNCTION__, __LINE__, _XSTRING(_expr));           \
        fprintf(stderr, _XSTRING(_msg) "\n", ##args);	            \
        exit(1);						                            \
    }								                                \
} while(0)

// called when <syscall> fails, print <msg> and the Unix perror value and die.
#define sys_die(syscall, msg, args...) do {                                         \
    debug("FATAL syscall error: " _XSTRING(msg) "\n\tperror reason: ", ##args);     \
    perror(_XSTRING(syscall));                                                      \
    exit(1);                                                                        \
} while(0)

// the called code cannot fail (<0)
#define no_fail(syscall) do {                                               \
    if((syscall) < 0)                                                       \
        sys_die(syscall, "<%s> failed: we thought this was impossible.\n",  \
                                                    _XSTRING(syscall));     \
} while(0)

// print msg, exit cleanly.
#define clean_exit(msg...) do { 						                            \
    fprintf(stderr, ##msg);						                            \
    exit(0);                                                                \
} while(0)

// error: die without prepending information.
#define die(msg...) do { 						                            \
    fprintf(stderr, ##msg);						                            \
    exit(1);                                                                \
} while(0)


// die with a panic message; prepend file,function,linenumber.
#define panic(msg...) do { 						                            \
    output("%s:%s:%d:PANIC:", __FILE__, __FUNCTION__, __LINE__);   \
    die(msg); \
} while(0)


// same as panic, but differentiate that we literally thought this
// could not happen (not just that we were simply not handling a 
// case, etc.) maybe should refactor.
#define impossible(args...) do {                            \
    fprintf(stderr, "%s:%s:%d:IMPOSSIBLE ERROR:",           \
                        __FILE__, __FUNCTION__, __LINE__);  \
    fprintf(stderr, ##args);						        \
    exit(1);                                                \
} while(0)

// print a debug message; prepend file:function:linenumber.
#define debug(msg...) do { 						                    \
    fprintf(stderr, "%s:%s:%d:", __FILE__, __FUNCTION__, __LINE__); \
    fprintf(stderr, ##msg);						                    \
} while(0)

// output to stderr without attaching the file/line.
#define output(msg...) fprintf(stderr, ##msg)

#define notreached()    panic("impossible: hit NOTREACHED!\n")
#define unimplemented() panic("this code is not implemented.\n");

/* Compile-time assertion: can only be used in a function. */
#define AssertNow(x) switch(1) { case (x): case 0: ; }

#endif /* __DEMAND_H__ */
