#ifndef __ESP_COMMANDS_H__
#define __ESP_COMMANDS_H__
/*************************************************************************
 * esp commands: these are defined by you.
 */

// just see if esp responds.
int esp_is_up(esp_t *e);

// either setup wifi or connect first.
int esp_setup_wifi(esp_t *e);
int esp_connect_to_wifi(esp_t *e);

// then setup a tcp server or client.
int esp_start_tcp_server(esp_t *e, unsigned portnum);
int esp_start_tcp_client(esp_t *e, const char *server_ip, unsigned portn);

int esp_hard_reset(esp_t *e);

int esp_send(esp_t *e, unsigned ch, const void *data, unsigned n);

int esp_connect(esp_t *e);

int wait_for_conn(esp_t *e, unsigned *ch);

// make another one that is no copy.
int esp_is_recv(esp_t *e, unsigned *ch, void *data, unsigned n);

#endif
