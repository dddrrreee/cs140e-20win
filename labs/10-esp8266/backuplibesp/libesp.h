#ifndef __LIBESP_H__
#define __LIBESP_H__
// system specific definitions.
#include <string.h>

#ifdef RPI_UNIX
#   include "unix-support.h"
#elif  RPI_OS
#   include "pi-support.h"
#else
#   error "not handling!"
#endif

// needs <esp_handle_t>
#include "esp-parse.h"

// gather esp stuff into one place.
// a bit incoherent since it conflates:
//  1. access point information.
//  2. server info.
//  3. client info
//  different connections.
#include "esp-constants.h"

// client side connections.
typedef struct esp_conn {
    // is this a client or server connection?
    // client has more information.
    unsigned client_p:1,
             active_p;
    
    enum { ESP_TCP = 13, ESP_UDP } type;

    // client info
    const char *server_ip;
    unsigned portn;
} esp_conn_t;

typedef struct esp {
    lex_t *l;

    const char *wifi;
    const char *password;

    int (*handle_ooo_msg)(struct esp *self);

    // has been connected to wife, and a server/client started.
    unsigned active_p;

    esp_conn_t  open_conn[ESP_MAX_CONN];
} esp_t;


static inline esp_t esp_mk(lex_t *l, const char *wifi, const char *password) {
    int default_handle_out_of_order_msg(struct esp *e);

    return (esp_t) { 
        .l = l, 
        .wifi = wifi, 
        .password = password, 
        .active_p = 0,
        .handle_ooo_msg = default_handle_out_of_order_msg 
    };
}

#include "esp-commands.h"
#include "esp-run.h"

// user code can define this and then get called automatically
// with <l> setup.
void esp_main(lex_t *l);

/*************************************************************************
 * client must define these routines.
 */
int esp_has_data_timeout(lex_t *l, unsigned usec);

// read in at most <maxbytes> of data into <buf>
// returns:
//  - number of bytes read on success.
//  - 0 if timed out after <usec>
//  - -1 if there was an error.
int esp_read(lex_t *l, void *buf, unsigned maxbytes, unsigned usec);

// returns <n> if wrote, 0 otherwise.
int esp_write_exact(lex_t *l, const void *buf, unsigned n);

// sleep for <usec> microseconds.
int esp_usleep(unsigned usec);


#endif
