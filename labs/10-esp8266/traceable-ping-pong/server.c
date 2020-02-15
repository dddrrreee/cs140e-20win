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

/*
static const char *network = "no_step_on_snek";
static const char *password = "foobarbaz";
*/

static int vm [1024];

// Added code 
// 1 is put 
// 0 is get
typedef struct {
	uint32_t op;
	uint32_t loc;
	uint32_t val; 
} protobuf_t; 

protobuf_t make_put(uint32_t loc, uint32_t val) {
	protobuf_t protobuf;
	protobuf.op = 1;
	protobuf.loc = loc;
	protobuf.val = val;
	return protobuf;
}

protobuf_t make_get(uint32_t loc, uint32_t val) {
	protobuf_t protobuf;
	protobuf.op = 0;
	protobuf.loc = loc;
	protobuf.val = val;
	return protobuf;
}

int parse_cmd (protobuf_t protobuf, esp_t* e) {
	unsigned ch = 0;
	switch(protobuf.op) {
		case 0: //get
			output("GET32: %u, %u\n", protobuf.loc, protobuf.val); 
			protobuf_t get_protobuf = make_get(protobuf.loc, vm[protobuf.loc]);
			esp_send(e, ch, &get_protobuf, sizeof(get_protobuf));
			break;
		case 1: //put
			output("PUT32: %u, %u\n", protobuf.loc, protobuf.val);
			protobuf_t put_protobuf = make_put(protobuf.loc, protobuf.val);
			vm[protobuf.loc] = protobuf.val;
			esp_send(e, ch, &put_protobuf, sizeof(put_protobuf));
			break;
	}
	return 1;

}
void serve_requests(esp_t* e) {
	unsigned ch = 0;
	protobuf_t incoming_protobuf;
	
	unsigned status = esp_is_recv(e, &ch, &incoming_protobuf, sizeof(incoming_protobuf));
	//while (status <= 0) {
	//	unsigned status = esp_is_recv(e, &ch, &incoming_protobuf, sizeof(incoming_protobuf));
	//}
	parse_cmd(incoming_protobuf, e);
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

	while(1) {
		serve_requests(&e);
	}
}
