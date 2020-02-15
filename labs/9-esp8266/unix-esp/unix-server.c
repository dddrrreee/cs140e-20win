#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>

#include "libesp.h"
#include "libunix.h"

// static const char *network = "engler123";

// do your own network id, using email most likely
static const char *network = "bjun2@stanford.edu";
static const char *password = "12345678";

int main(int argc, char *argv[]) { 
    const char *dev_name = 0;
    if(argc == 1)
        dev_name = find_ttyusb_last();
    else if(argc == 2)
        dev_name = argv[1];
    else
        panic("too many arguments %d\n", argc);

    output("opening: <%s>\n", dev_name);
    int fd = set_tty_to_8n1(open_tty(dev_name), B115200, 1);

    int log_fd = create_file("log-esp.txt");
    esp_log(log_fd);

    lex_t l;
    lex_init(&l, fd);
    esp_t e = esp_mk(&l, network, password);

    if(!esp_is_up(&e))
        panic("esp is not running!\n");
    unsigned port = ESP_DEFAULT_PORT;

    esp_hard_reset(&e);
    if(!esp_setup_wifi(&e))
        panic("can't setup the wifi network!\n");
    if(!esp_start_tcp_server(&e, port))
        panic("can't setup the wifi network!\n");

    int ch;
    if((ch = esp_connect(&e)) < 0)
        panic("no connection\n");

    // print out any data we receive.
    for(int i = 0; i < 10; i++)  {
        // send hello here.
        esp_send(&e, 0, "Hello", 5);
        sleep(1);
    }
    return 0;
}
