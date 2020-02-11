#ifndef __ESP_CONSTANTS_H__
#define __ESP_CONSTANTS_H__

// currently assuming this is always the server
// IP address: can pull it out of the AT+CIFSR
#define ESP_SERVER_IP "192.168.4.1"

// just use the same port num
#define ESP_DEFAULT_PORT 4444

// firmware only allows small number of connections.
#define MAX_ESP_CONN 6

// can mess around with this.
#define ESP_MAX_PKT  1024

#endif
