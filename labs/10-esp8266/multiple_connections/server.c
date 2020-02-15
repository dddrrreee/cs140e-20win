#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>

#include "libesp.h"
#include "libunix.h"

static const char *network = "vikasm2";
static const char *password = "12345678";

static int is_connect(esp_t *e, unsigned *ch) {
    // blocks until input: should check if its there?
    if(!match_line(e->l, "%u, CONNECT", ch))
        return 0;
    assert(*ch < ESP_MAX_CONN);
    return 1;
}

static int is_disconnect(esp_t *e, unsigned *ch) {
    // blocks until input: should check if its there?
    if(!match_line(e->l, "%u, DISCONNECT", ch))
        return 0;
    assert(*ch < ESP_MAX_CONN);
    return 1;
}

void esp_main(lex_t *l) {
    esp_t e = esp_mk(l, network, password);

#if 0
    if(!esp_is_up(&e))
        panic("esp is not running!\n");
    output("esp is up and running!\n");
    output("esp is up and running!\n");
#endif
    unsigned port = ESP_DEFAULT_PORT;

    // can we tell if the AP is up?
    esp_hard_reset(&e);
    if(!esp_setup_wifi(&e))
        panic("can't setup the wifi network!\n");
    if(!esp_start_tcp_server(&e, port))
        panic("can't setup the wifi network!\n");

    // this is broken: you need to do async.
    output("Going to wait for a connection\n");
    unsigned ch;
    while(!wait_for_conn(&e, &ch)) {
        cmd_echo_line(&e, "no connection");
        esp_usleep(1000);
    }
    output("connected!\n");

#if 0
    if(!esp_start_tcp_client(&e, ESP_SERVER_IP, port))
        panic("can't setup the wifi network!\n");
    output("going to start sending!\n");
#endif

    esp_set_verbose(0);
    unsigned x,n;
    // print out any data we receive.
    while(1) {
        char received [1024];
        unsigned ch = 0;
        
		while(!is_connect(&e, &ch) && !is_disconnect(&e, &ch)) {
            esp_is_recv(&e, &ch, received, sizeof(received));
		}
		
		unsigned n = is_connect(&e, &ch);
        
		received[n] = 0;
        
        if(!esp_send(&e, ch, received, strlen(received))){
            panic("Cannot send\n");
        }
    }
}
