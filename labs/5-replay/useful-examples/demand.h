#ifndef __DEMAND_H__
#define __DEMAND_H__

#include <stdio.h>
#include <stdlib.h>

#define _XSTRING(x) #x

// usage: 
//	demand(expr, msg)
// prints <msg> if <expr> is false.
// example:
//	demand(x < 3, x has bogus value!);
// note: 
//	if <_msg> contains a ',' you'll have to put it in quotes.
#define demand(_expr, _msg) do {					                \
    if(!(_expr)) { 							                        \
        fprintf(stderr, "ERROR:%s:%s:%d: "			                \
            "FALSE(<" _XSTRING(_expr) ">): " _XSTRING(_msg) "\n",   \
                __FILE__, __FUNCTION__, __LINE__);		            \
        exit(1);						                            \
    }								                                \
} while(0)

// called when <syscall> fails, print <msg> and the Unix perror value and die.
#define sys_die(syscall, msg) do {                                  \
    fprintf(stderr, "%s:%s:%d: <%s>\n\tperror=",                    \
                __FILE__, __FUNCTION__, __LINE__, _XSTRING(msg));   \
    perror(_XSTRING(syscall));                                      \
    exit(1);                                                        \
} while(0)

// die with a panic message; prepend file,function,linenumber.
#define panic(msg...) do { 						\
        fprintf(stderr, "%s:%s:%d:", __FILE__, __FUNCTION__, __LINE__); \
        fprintf(stderr, ##msg);						\
        exit(1);                                                        \
} while(0)

// print a debug message; pre-pend file:function:linenumber.
#define debug(msg...) do { 						                    \
    fprintf(stderr, "%s:%s:%d:", __FILE__, __FUNCTION__, __LINE__); \
    fprintf(stderr, ##msg);						                    \
} while(0)

// output to stderr without attaching the file/line.
#define output(msg...) fprintf(stderr, ##msg)

/* Compile-time assertion: can only be used in a function. */
#define AssertNow(x) switch(1) { case (x): case 0: ; }

#endif /* __DEMAND_H__ */
