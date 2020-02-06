// test the new gpio routines.
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "rpi.h"
#include "pi-test.h"

#define stringify(x) #x
#define test(fn,ntrials) do {                           \
    printf("testing <%s>\n", stringify(fn));            \
    /* test pins 0..32, then a bunch of random. */      \
    for(int i = 0; i < 32; i++)                         \
        fn(i);                                          \
    for(int i = 0; i < ntrials; i++)                    \
        fn(fake_random());                              \
} while(0)

int main(int argc, char *argv[]) {
    // make sure that everyone has the same fake_random.
    fake_pi_init();

#   define N 128
    test(gpio_int_rising_edge, N);
    test(gpio_int_falling_edge, N); 
    test(gpio_event_detected, N); 
    test(gpio_event_clear, N);
    return 0;
}
