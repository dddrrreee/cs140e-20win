#include "rpi.h"
#include "rpi-thread.h"

// test that we can yield and do a simple realtime thing.
struct pwm {
    unsigned duty;  // between 0 and 100
    unsigned pin;
};

// wait for <n> from the current time.  we yield
// until enough time passes.  if yield is fast and
// other threads don't run too long, this can be
// accurate real-time, even though we don't have 
// an actual real-time scheduler.
static void wait_usec(unsigned n) {
    demand(n < 100000, "unlikely large delay = %dusec!\n", n);
    unsigned start = timer_get_usec();
    while(1) {
        if((timer_get_usec() - start) < n)
            return;
        rpi_yield();
    }
}

// if our threads are fast enough you can run a bunch of these
// and not see weird flickers.
void blink(void *arg) {
    struct pwm *p = arg;

    gpio_set_output(p->pin);
    demand(p->duty > 0 && p->duty <= 100, wierd duty cycle!);
    
    // how long we should be on out of 100 usec.
    unsigned on_usec = p->duty;
    unsigned off_usec = 100 - p->duty;

    for(int i = 0; i < 1000; i++) {
        gpio_set_on(p->pin);
        wait_usec(on_usec);
        
        gpio_set_off(p->pin);
        wait_usec(off_usec);
    }
}

void notmain(void) {
    uart_init();
    kmalloc_init();

    struct pwm t_75 = {.duty = 75, .pin = 20 },
               t_25 = {.duty = 25, .pin = 21 };

    rpi_fork(blink, &t_75);
    rpi_fork(blink, &t_25);
    rpi_thread_start();
    printk("done\n");
    clean_reboot();
}
