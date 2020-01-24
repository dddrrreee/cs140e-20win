// engler: simple example of writing pins for a given number of cycles.
#include "rpi.h"
#include "cycle-util.h"

void notmain(void) {
    int tx = 21;
    gpio_set_output(tx);
    gpio_write(tx,0);

    cycle_cnt_init();

    // seems to help: try with and without.
    enable_cache();

    // pick some arbitrary number of cycles to write (not too small unless
    // you're going to get fancy.)
    unsigned n = 7777;
    printk("about to start sending bits: should delay <%d>cyc\n", n);

    // note: if you look at the assembly of the code below, you'll see gcc is
    // extremely stupid about large contants.  rather than putting in a register
    // it keeps loading it from memory.  given that we run uncached, this is slow.
    // fixing this (i had to pass the values as arguments to a function in another
    // file) is kinda annoying, but does reduce variance.  there is probably
    // a better gcc-specific way.
    unsigned end = n;
    unsigned start = cycle_cnt_read();

    write_cyc_until(tx, 1, start, end);  end += n;   // 1
    write_cyc_until(tx, 0, start, end);  end += n;   // 2
    write_cyc_until(tx, 1, start, end);  end += n;   // 3
    write_cyc_until(tx, 0, start, end);  end += n;   // 4
    write_cyc_until(tx, 1, start, end);  end += n;   // 5
    write_cyc_until(tx, 0, start, end);  end += n;   // 6
    write_cyc_until(tx, 1, start, end);  end += n;   // 7
    write_cyc_until(tx, 0, start, end);  end += n;   // 8
    write_cyc_until(tx, 1, start, end);  end += n;   // 9
    write_cyc_until(tx, 0, start, end);  end += n;   // 10

    printk("done\n");
    clean_reboot();
}
