// simple driver for replaying a trace.
// example:
//   ./replay ./log.interpose.txt my-install ./hello.bin
// 
// first get the successful run working.  then do the corruption by removing the
// #if 0
//
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <string.h>
#include <unistd.h>
#include "libunix.h"
#include "interpose.h"

void usage(void) {
    output("Usage error: `replay <log-file-name> <unix-side-install> <program-name>\n");
    output("E.g.   replay ./log-file.txt my-install ./hello.bin\n");
    exit(1);
}

#include "shared-code/simple-boot.h"
static int log_eq32(log_ent_t *e, uint32_t v) {
    uint8_t sender = e[0].sender;

    for(int i = 0; i < 4; i++) 
        if(e[i].sender != sender)
            return 0;

    uint8_t v0 = e[0].v;
    uint8_t v1 = e[1].v;
    uint8_t v2 = e[2].v;
    uint8_t v3 = e[3].v;

    uint32_t vv = (v3 << 24 | v2 << 16 | v1 << 8 | v0);
    // output("comparing %x to %x\n", v, vv);
    return v == vv;
}

int main(int argc, char *argv[]) {
    if(argc < 4) {
        output("too few arguments: %d\n", argc);
        usage();
    }
    char *logfile = argv[1];
    char *unix_side = argv[2];
    char *pi_prog = argv[3];

    unsigned n;
    log_ent_t *log = log_parse_tuples(logfile,&n);

    // gross calculation to figure out the last byte we can corrupt.
    int ncorrupt = 0;
    for(int i = 0; i < n-4; i++) {
        if(log[i].sender != PI_OUTPUT)
            continue;
        if(log_eq32(log+i,BOOT_SUCCESS)) {
            ncorrupt += 3;
            output("found it! iter=%d, ncorrupt=%d\n", i, ncorrupt);
            break;
        }
        ncorrupt++;
    }

    int ntrials = 1;
    // run successful trials.
    for(int i = 0; i < ntrials; i++)  {
        endpt_t u = start_unix_side(unix_side, pi_prog);
        replay_unix(&u, log, n, -1);
    }

    // these are the corruption runs.
    for(int i = 0; i < ncorrupt; i++) {
        if(i < 4)
            continue;
        output("-------------------TRIAL %d about to run-----------------------------\n",i);
        output("going to try to corrupt %d\n", i);
        endpt_t u = start_unix_side(unix_side, pi_prog);
        if(!replay_unix(&u, log, n, i))
            panic("impossible: did not corrupt i=%d\n", i);
        close_open_fds();
        usleep(10000);
        output("-------------------TRIAL %d was a success-----------------------------\n",i);
    }
    output("we checked %d corruption trials and all failed as expected\n", ncorrupt);
    return 0;
}
