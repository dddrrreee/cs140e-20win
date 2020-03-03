// simple driver for tracing a bootloading program.  just gets arguments, does
// checking and then calls <interpose>
// 
// invoked either via:
//      trace my-install <ttyusb-dev> <program-name>
// or
//      trace my-install <program-name>
// where the ttyusb device has to be discovered.
//
// should also work with pi-install, or whatever you change the program to.  main issue
// is just picking a different directory to put the traces in.
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <string.h>
#include <unistd.h>
#include "libunix.h"
#include "interpose.h"


void usage(void) {
    output("Usage error: `interpose <unix-side-install> [/dev/device] <program-name>\n");
    output("E.g. to run `foo` on tty-USB device `/dev/ttyUSB0`:\n");
    output("        interpose my-install /dev/ttyUSB0 hello.bin\n");
    exit(1);
}

static void emit_if_uniq(const char *dir, uint8_t *p, unsigned nbytes) {
    if(mkdir(dir, 0700) < 0) {
        if(errno != EEXIST)
            sys_die(mkdir, mkdir failed: weird);
    }

    uint32_t crc = our_crc32(p,nbytes);
    char buf[1024];
    sprintf(buf, "%s/log-file.%x.txt", dir, crc);

    if(open(buf, O_RDONLY) > 0) {
        output("already saw execution <%s>\n", buf);
        return;
    }
    output("New execution!   Creating file <%s>\n", buf);
    int fd = create_file(buf);
    write_exact(fd, p, nbytes);
    close(fd);
}

int main(int argc, char *argv[]) {
#if 1
    char *bootloader = argv[1];
    char **argv_rest = 0;
    char *dev_name = 0;

    if(argc < 3)
        die("ERROR: wrong number (n=%d) of arguments to %s\n", argc, argv[0]);
    else if(strncmp(argv[2], "/dev", 4) != 0) {
        argv_rest = &argv[2];
        dev_name = find_ttyusb();
    } else {
        if(argc < 4)
            panic("too few arguments\n");
        dev_name = argv[2];
        argv_rest = &argv[3];
    }
#else
    char *bootloader = argv[1];
    char *pi_prog = argv[argc-1];
    char *dev_name = 0;

    // interpose my-install foo.bin
    if(argc == 3)
        dev_name = find_ttyusb();
    // interpose pi-install /dev/ttyUSB foo.bin
    else if(argc == 4) {
        dev_name = argv[argc-2];
        const char *prefix = "/dev/";
        if(strncmp(dev_name, prefix, strlen(prefix)) != 0)
            die("ERROR: unexpected device=<%s>, expected a <%s> prefix\n", dev_name, prefix);
    } else
        die("ERROR: wrong number (n=%d) of arguments to %s\n", argc, argv[0]);
#endif    
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
}

