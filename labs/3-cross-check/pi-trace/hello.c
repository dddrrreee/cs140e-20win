// example client that uses our tracing system: can cross check these values
// against the ones on your laptop!
#include "rpi.h"
#include "trace.h"

void notmain(void) {
    uart_init();

    printk("about to trace gpio_read(20):\n");
    trace_start(0);
    int v = gpio_read(20);
    trace_stop();
    printk("done: gpio_read(20)=%d\n", v);

    // make the above nicer so we can easily trace a function/chunk of code.
#   define stringify(_x) #_x

#   define trace_fn(fn) do {                                    \
        printk("about to trace: <%s>\n", stringify(fn));        \
        trace_start(0);                                         \
        fn;                                                     \
        trace_stop();                                           \
        printk("done: <%s>\n", stringify(fn));                  \
    } while(0)

    // you can easily trace anything. 
    //
    // i would *ALWAY* recommend doing this approach when you rewrite something
    // to make it cleaner.
    //   1. run working code before you modify to get a reference trace.
    //   2. incrementally modify, re-tracing and checking.
    //   3. profit.
    //
    // a huge amount of embedded code can be improved in this way.  
    // start with working-but-ugly.  rewrite to working-and-clean while
    // verifing they do exactly the same read/writes to exactly the sme
    // locations/values in exactly the same order.  
    trace_fn(gpio_read(20));
    trace_fn(gpio_set_input(20));
    trace_fn(gpio_set_output(20));

    clean_reboot();
}
