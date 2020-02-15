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
#include <unistd.h>

static const char *network = "no_step_on_python";
static const char *password = "foobarbaz";

typedef struct {
  uint32_t eot;
  char buf[64];
} message_t;

void esp_main(lex_t *l) {
#if 1
  esp_t e = esp_mk(l, network, password);

  // if you control-c the code running on unix, that has no
  // impact on the esp.  we hard reset it to try to put it
  // in a "clean" initial state.
  esp_hard_reset(&e);
  if (!esp_connect_to_wifi(&e))
    panic("cannot connect to tcp client\n");

  unsigned port = ESP_DEFAULT_PORT;
  const char *ip = ESP_SERVER_IP;
  if (!esp_start_tcp_client(&e, ip, port))
    panic("can't start esp tcp client\n");

  esp_set_verbose(0);
#endif

  unsigned ch = 0;

  while (1) {
    while (1) {
      message_t incoming;
      esp_is_recv(&e, &ch, &incoming, sizeof(incoming));
      incoming.buf[sizeof(incoming.buf) - 1] = 0;
      write(2, incoming.buf, strlen(incoming.buf));
      if (incoming.eot)
        break;
    }
    /* fprintf(stderr, "\n> "); */
    char query[64];
    for (int i = 0; i < sizeof(query); i++) {
      query[i] = 0;
    }
    ssize_t x = read(0, &query, sizeof(query));
    if (x == sizeof(query)) {
      fprintf(stderr, "command too long\n");
      continue;
    }
    query[x - 1] = '\n';
    query[x] = 0;
    message_t message;
    message.eot = 0;
    strcpy(message.buf, query);
    esp_send(&e, ch, &message, sizeof(message));
    message.eot = 1;
    esp_send(&e, ch, &message, sizeof(message));
  }
}
