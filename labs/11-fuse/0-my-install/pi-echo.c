#include <assert.h>
#include "libunix.h"
int main(void) {
    if(!fd_is_open(HANDOFF_FD))
        panic("pi-echo: must run with <HANDOFF_FD> as an open file descriptor\n");
    pi_echo(0, HANDOFF_FD, 0);
    return 0;
}
