#undef assert

#ifndef __RPI_ASSERT_H__
#define __RPI_ASSERT_H__

#ifndef NDEBUG
#define debug(msg, args...) \
	(printk)("%s:%s:%d:" msg, __FILE__, __FUNCTION__, __LINE__, ##args)
#else
#define debug(msg, args...) do { } while(0)
#endif

#define output printk

#define panic(msg, args...) do { 					\
    rpi_reset_putc();\
	(printk)("PANIC:%s:%s:%d:" msg "\n", __FILE__, __FUNCTION__, __LINE__, ##args); \
	clean_reboot();							\
} while(0)

#define pi_trace(args...) do { printk("TRACE:"); printk(args); } while(0)

#define assert(bool) do { if((bool) == 0) panic(#bool); } while(0)

#define _XSTRING(x) #x

// usage: 
//      demand(expr, msg)
// note, if it doesn't take special characters does not need quotes:
//      demand(x, x is 0!);
// but you can always add them: 
//      demand(x, "x is not 0 = %d", x);
// prints <msg> if <expr> is false.
// example:
//      demand(x < 3, x has bogus value!);
// note: 
//      if <_msg> contains a ',' you'll have to put it in quotes.
#define demand(_expr, _msg, args...) do {                                        \
    if(!(_expr)) {                                                  \
        rpi_reset_putc();\
        printk("ERROR:%s:%s:%d: "                      \
                        "FALSE(<" _XSTRING(_expr) ">): " _XSTRING(_msg) "\n",\
                        __FILE__, __FUNCTION__, __LINE__, ##args);              \
        clean_reboot();\
    }                                                               \
} while(0)

/* Compile-time assertion used in function. */
#define AssertNow(x) switch(1) { case (x): case 0: ; }

#define unimplemented() panic("implement this function!\n");

#endif
