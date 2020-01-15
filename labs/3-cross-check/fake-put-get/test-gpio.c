
// simple test for your put/get implementation.
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "rpi.h"

void run_test(const char *name, void (*fp)(unsigned), int ntrials) {
    printf("testing: <%s>\n", name);
    // test pins 0..32, then a bunch of random.
    for(int i = 0; i < 32; i++) 
        fp(i);
    for(int i = 0; i < ntrials; i++) 
        fp(random());
}

// can refactor this code, but we keep it simple.
void test_gpio_set_output(int ntrials) {
    run_test(__FUNCTION__, gpio_set_output, ntrials);
}
void test_gpio_set_input(int ntrials) {
    run_test(__FUNCTION__, gpio_set_input, ntrials);
}
void test_gpio_set_on(int ntrials) {
    run_test(__FUNCTION__, gpio_set_on, ntrials);
}
void test_gpio_set_off(int ntrials) {
    run_test(__FUNCTION__, gpio_set_off, ntrials);
}

void test_gpio_set_function(int ntrials) {
    printf("testing: <%s>\n", __FUNCTION__);
    // test pins 0..32, then a bunch of random.
    for(int pin = 0; pin < 32; pin++)  {
        for(int func = 0;  func < 16; func++) {
            gpio_set_function(pin, func);
            gpio_set_function(random(), func);
        }
    }
    for(int i = 0; i < ntrials; i++) 
        gpio_set_function(random(), random()%6);
}

int main(int argc, char *argv[]) {
    // make sure that everyone has the same random.
    assert(1450224440 == random());

#   define N 128
    if(argc > 2) {
        fprintf(stderr, "usage error: test-gpio [0..3]\n");
        exit(1);
    }
    int part = 0;
    if(argv[1])
        part = atoi(argv[1]);

    switch(part) {
    case 0: test_gpio_set_output(N); break;
    case 1: test_gpio_set_on(N); break;
    case 2: test_gpio_set_off(N); break;
    case 3:
            test_gpio_set_output(N); 
            test_gpio_set_on(N); 
            test_gpio_set_off(N);
            break;
    case 4: 
            //test_gpio_set_function(N);
            test_gpio_set_input(N);
            break;

    default: 
            fprintf(stderr, "invalid part<%s>\n", argv[1]);
            exit(1);
    }
    return 0;
}
