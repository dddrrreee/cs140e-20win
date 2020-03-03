#include "rpi.h"
#include "rpi-thread.h"
#include "circular.h"

struct end {
    cq_t *out, *in;
};

void pingpong(void *arg) {
    struct end *e = arg;

    unsigned x;
    while(x < 100) {
        while(!cq_pop_n_noblk(e->in, &x, sizeof x))
            rpi_yield(); 
        printk("received %d\n", x);
        if(!cq_push_n(e->out, &x, sizeof x))
            panic("impossible\n");
    }
    rpi_exit(0);
}

void notmain(void) {
    uart_init();
    kmalloc_init();

    cq_t q0,q1;
    cq_init(&q0, 1);
    cq_init(&q1, 1);

    unsigned x = 0;
    cq_push_n(&q0, &x, sizeof x);

    struct end e0 = {.out = &q0, .in = &q1 },
               e1 = {.out = &q1, .in = &q0 };

    rpi_fork(pingpong, &e0);
    rpi_fork(pingpong, &e1);
    rpi_thread_start();
    panic("should never return!\n");

    clean_reboot();
}
