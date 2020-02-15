#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>

#include "libesp.h"
#include "libunix.h"

static const char *network = "bjun2server";
static const char *password = "12345678";

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
    for(int i = 0; ; i++)  {
        char hello[1024];
        sprintf(hello, "hello %d", i);
        output("sending <%s>\n", hello);
        if(!esp_send(&e, 0, hello, strlen(hello)))
            panic("cannot send!!\n");

        output("going to wait on ch=%d\n", ch);
        // oh: we don't actually know
        char reply[1024];
        n = esp_is_recv(&e, &x, reply, sizeof reply);
        assert(x == ch);
        reply[n] = 0;
        output("received ack from client=%d: <%s>\n", ch, reply);
        if(strcmp(reply,hello) == 0)
            output("passed check!\n");
        else
            panic("mismatch: have <%s>, expected <%s>\n", reply,hello);

//        sleep(1);
    }
}
