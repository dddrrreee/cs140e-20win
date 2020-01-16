#ifndef __FAKE_TEST_H__
#define __FAKE_TEST_H__


unsigned long rpi_rand32(void);
static unsigned long fake_random(void) {
    return rpi_rand32();
}

static void fake_random_seed(unsigned x) {
}

// check that we get what we expect.
static void fake_random_check(void) {
    // make sure that everyone has the same random.
    assert(1450224440 == fake_random());
}

#endif
