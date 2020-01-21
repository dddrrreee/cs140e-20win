#ifndef __FAKE_TEST_H__
#define __FAKE_TEST_H__

// call this first.
void fake_pi_init(void);


// define nop to an actual nop.
// #define nop() do { } while(0)

unsigned long fake_random(void);

void fake_random_seed(unsigned x);

// have to call to setup.
void fake_time_init(unsigned init_time);

#define __RPI_ASSERT_H__
#include "rpi.h"

#include "demand.h"

// should use stderr, but it's irritating to capture the output.
#define trace(msg, args...) do {                                    \
    fprintf(stdout, "TRACE:%s:" msg, __FUNCTION__, ##args);          \
    fflush(stdout);                                                 \
} while(0)

void notmain(void);

#define random "don't call random: use fake_random"

#endif
