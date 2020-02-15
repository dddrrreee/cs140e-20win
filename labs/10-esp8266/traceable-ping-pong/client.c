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

/*
static const char *network = "no_step_on_snek";
static const char *password = "foobarbaz";
*/

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

void PUT32(esp_t* e, uint32_t loc, uint32_t val) {
    unsigned ch = 0;
	protobuf_t put_protobuf = make_put(loc, val);
    protobuf_t get_protobuf = make_get(loc, val);
    esp_send(e, ch, &put_protobuf, sizeof(put_protobuf));
    esp_is_recv(e, &ch, &get_protobuf, sizeof(get_protobuf));
}

void GET32(esp_t* e, uint32_t loc, uint32_t val) {
	unsigned ch = 0;
    protobuf_t get_protobuf = make_get(loc, val);
    protobuf_t put_protobuf = make_put(loc, val);
    esp_send(e, ch, &get_protobuf, sizeof(get_protobuf));
    esp_is_recv(e, &ch, &get_protobuf, sizeof(get_protobuf));
}

int parse_cmd (protobuf_t protobuf) {
    switch(protobuf.op) {
        case 0: //get
            printf("GET32: %u, %u", protobuf.loc, protobuf.val);
            break;
        case 1: //put
            printf("PUT32: %u, %u", protobuf.loc, protobuf.val);
            break;
    }
    return 1;

}

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
        PUT32(&e, 0x00000001, 0x00000022);
        PUT32(&e, 0x00000002, 0x00000044);
        GET32(&e, 0x00000003, 0x00000055);
		GET32(&e, 0x00000004, 0x000000ee);
		//unsigned ch, n = esp_is_recv(&e, &ch, buf, sizeof buf);
        //buf[n] = 0;
        //output("client ch=%d received: <%s>, going to ack\n", ch, buf);
        //if(!esp_send(&e, 0, buf, strlen(buf)))
        //    panic("cannot send!!\n");
    }
}
