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

    uint32_t crc = crc32(p,nbytes);
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

    endpt_t p = start_pi_side(dev_name);
    endpt_t u = start_unix_side(bootloader, pi_prog);

    const char *name = "./log.interpose.txt";
    int fd = create_file(name);
    trace(fd, &u, &p);
    close(fd);

    unsigned nbytes;
    uint8_t *t = read_file(&nbytes, name);
    const char *log_dir = "./log-files";
    emit_if_uniq(log_dir, t,nbytes);
    return 0;
}
