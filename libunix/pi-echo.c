#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include "libunix.h"

// hack-y state machine to indicate when we've seen the special string
// 'DONE!!!' from the pi telling us to shutdown.
int pi_done(unsigned char *s) {
    static unsigned pos = 0;
    const char exit_string[] = "DONE!!!\n";
    const int n = sizeof exit_string - 1;

    for(; *s; s++) {
        assert(pos < n);
        if(*s != exit_string[pos++]) {
            pos = 0;
            return pi_done(s+1); // check remainder
        }
        // maybe should check if "DONE!!!" is last thing printed?
        if(pos == sizeof exit_string - 1)
            return 1;
    }
    return 0;
}

// overwrite any unprintable characters with a space.
// otherwise terminals can go haywire/bizarro.
// note, the string can contain 0's, so we send the
// size.
void remove_nonprint(uint8_t *buf, int n) {
    for(int i = 0; i < n; i++) {
        uint8_t *p = &buf[i];
        if(isprint(*p) || (isspace(*p) && *p != '\r'))
            continue;
        *p = ' ';
    }
}

// read and echo the characters from the usbtty until it closes 
// (pi rebooted) or we see a string indicating a clean shutdown.
void pi_echo(int unix_fd, int pi_fd, const char *portname) {
    assert(pi_fd);
    if(portname)
        output("listening on ttyusb=<%s>\n", portname);

    while(1) {
        unsigned char buf[4096];

        int n;
        if((n=read_timeout(unix_fd, buf, sizeof buf, 1000))) {
            buf[n] = 0;
            write_exact(pi_fd, buf, n);
        }

        if(!can_read_timeout(pi_fd, 1000))
            continue;
        n = read(pi_fd, buf, sizeof buf - 1);

        if(!n) {
            // this isn't the program's fault.  so we exit(0).
            if(!portname || tty_gone(portname))
                clean_exit("pi ttyusb connection closed.  cleaning up\n");
            // so we don't keep banginging on the CPU.
            usleep(1000);
        } else if(n < 0) {
            sys_die(read, "pi connection closed.  cleaning up\n");
        } else {
            buf[n] = 0;
            remove_nonprint(buf,n);
            output("%s", buf);

            if(pi_done(buf)) {
                output("\nSaw done\n");
                clean_exit("\nbootloader: pi exited.  cleaning up\n");
            }
        }
    }
    notreached();
}
