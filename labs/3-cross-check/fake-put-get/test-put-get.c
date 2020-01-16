// simple test for your put/get implementation.
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "rpi.h"
#include "pi-test.h"


int main(void) {
    // make sure that everyone has the same random.
    fake_random_check();

#   define N 128

    // should the same as in test.out
    volatile void *addrs[N];
    unsigned vals[N];
    for(int i = 0; i < N; i++) {
        addrs[i] = (volatile void*)fake_random();

        if(fake_random()%2 == 0)
            vals[i] = get32(addrs[i]);
        else
            put32(addrs[i], vals[i] = fake_random());
    }
    for(int i = 0; i < N; i++)
        assert(get32(addrs[i]) == vals[i]);

    return 0;
}
