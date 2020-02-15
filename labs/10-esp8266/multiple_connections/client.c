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

static const char *network = "vikasm2";
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
    int i = 0;
	for(i = 0; i < 10; i++){
		char msg [1024];
        unsigned ch = 2;
		sprintf(msg, "Brian");
		output("Sending message <%s>\n", msg);
		if(!esp_send(&e, ch, msg, strlen(msg))){
			panic("Cannot send\n");
		}

		char received [1024];
        unsigned n = esp_is_recv(&e, &ch, received, sizeof(received));
		received[n] = 0;
		output("Client received %s:", received);

		while(1) {
			esp_usleep(6000000);
			output("Sleeping...\n");
		}
	}
}
