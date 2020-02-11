/*
 * put platform-specific routines for:
 *  - esp_read
 *  - exp_write_exact
 *  - esp_has_data_timeout.
 */
#include <unistd.h>
#include "libesp.h"

static int log_fd = 0;
void esp_log(int fd) {
    log_fd = fd;
}

// called when the lex buffer has no characters.  reads in more from the esp.
int esp_read(lex_t *l, void *buf, unsigned maxbytes, unsigned usec) {
    int r;

    if(!esp_has_data_timeout(l, usec))
        return 0;
    if((r = read(l->h, buf, maxbytes)) <= 0)
        sys_die(read, failed);

    // write out what we are reading.
    if(log_fd) 
        write(log_fd, buf, r);

    return r;
}

// returns <n> if wrote, 0 otherwise.
int esp_write_exact(lex_t *l, const void *buf, unsigned n) {
    int r;

    // should probably send this to the log as well.
    if((r=write(l->h, buf, n)) != n)
        panic("could not write <%d> bytes, have: %d\n",  n,r);

    return n;
}

int esp_has_data_timeout(lex_t *l, unsigned usec) {
    return can_read_timeout(l->h, usec);
}

// sleep for <usec> microseconds.
int esp_usleep(unsigned usec) {
    return usleep(usec);
}
