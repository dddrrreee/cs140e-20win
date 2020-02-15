/*
 * put platform-specific routines for:
 *  - esp_read
 *  - exp_write_exact
 *  - esp_has_data_timeout.
 */
#include "libesp.h"

// called when the lex buffer has no characters.  reads in more from the esp.
int esp_read(lex_t *l, void *buf, unsigned maxbytes, unsigned usec) {
    unimplemented();
}

// returns <n> if wrote, 0 otherwise.
int esp_write_exact(lex_t *l, const void *buf, unsigned n) {
    unimplemented();
    return n;
}

int esp_has_data_timeout(lex_t *l, unsigned usec) {
    unimplemented();
    return 0;
}

// sleep for <usec> microseconds.
int esp_usleep(unsigned usec) {
    delay_us(usec);
    return usec;
}
