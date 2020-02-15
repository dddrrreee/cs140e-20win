// engler, cs140e: client code for a simple hello ping pong.
// you should:
//  1. make a copy of the directory and change the code to do a PUT32, GET32.
//  2. rewrite so that you can handle multiple connections to the server.
//     you might have to modify some of the esp-command.c code.
//  3. add esp_command implementations that allow you to gracefully handle 
//     errors in send/receive ---- if you have a distributed system, then:
//           T(uptime) -> inf
//     and/or
//           N(nodes)->inf
//     then Pr(fail)=1.
//   4. make sure it runs on the pi.
#include "libesp.h"
#include "libunix.h"

static const char *network = "brian";
static const char *password = "12345678";

void esp_main(lex_t *l) {
    esp_t e = esp_mk(l, network, password);

    // if you control-c the code running on unix, that has no
    // impact on the esp.  we hard reset it to try to put it
    // in a "clean" initial state.
    esp_hard_reset(&e);
    if(!esp_connect_to_wifi(&e))
        panic("cannot connect to tcp client\n");

    unsigned port = ESP_DEFAULT_PORT;
    const char *ip = ESP_SERVER_IP;
    if(!esp_start_tcp_client(&e, ip, port))
        panic("can't start esp tcp client\n");

    esp_set_verbose(0);
    while(1) {
        esp_debug("going to receive\n");
        char buf[100];
        sprintf(buf, "https://pdos.csail.mit.edu/~engler/jr-calif.html");
		output("Yo!\n");
        if(!esp_send(&e, 0, buf, sizeof(buf)))
            panic("cannot send!!\n");

		output("DONE!\n");
		break;
    }
}
