// simple test for your put/get implementation.
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "rpi.h"

#define random (unsigned long)rpi_rand32

int main(void) {
    // make sure that everyone has the same random.
    assert(1450224440 == random());

#   define N 128

    // should the same as in test.out
    volatile void *addrs[N];
    unsigned vals[N];
    for(int i = 0; i < N; i++) {
        addrs[i] = (volatile void*)random();

        if(random()%2 == 0)
            vals[i] = get32(addrs[i]);
        else
            put32(addrs[i], vals[i] = random());
    }
    for(int i = 0; i < N; i++)
        assert(get32(addrs[i]) == vals[i]);

    return 0;
}
