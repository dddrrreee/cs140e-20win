
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>

#include "libesp.h"
#include "libunix.h"

int main(int argc, char *argv[]) {
    const char *dev_name = 0;
    if(argc == 1) {
        if(strstr(argv[0], "server")) 
            dev_name = find_ttyusb_first();
        else if(strstr(argv[0], "client")) 
            dev_name = find_ttyusb_last();
        else
            dev_name = find_ttyusb();
    }
    else if(argc == 2)
        dev_name = argv[1];
    else
        panic("too many arguments %d\n", argc);

    output("opening: <%s>\n", dev_name);
    int fd = set_tty_to_8n1(open_tty(dev_name), B115200, 1);

    char *p = argv[0] + strlen(argv[0]) - 1;
    for(; p > argv[0]; p--) {
        if(!isalpha(*p)) {
            p++;
            break;
        }
    }
    char *progname = p;
    int log_fd = create_file(strcatf("./log-esp.%s.txt", progname));
    esp_log(log_fd);

    lex_t l;
    lex_init(&l, fd);

    esp_main(&l);
    return 0;
}

