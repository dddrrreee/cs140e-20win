/*
 * engler, cs140e: routines to use the ESP.  put all your code here
 * and in its header (esp-commands.h).  do not modify anything else 
 * in case we need to push bug fixes.  sorry.
 */
#include <stdarg.h>
#include "libesp.h"

// connect, disconnect.   print wth messages.
// change the debugging.
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

// wait: what is the other stuff?
int wait_for_conn(esp_t *e, unsigned *ch) {
    return is_connect(e,ch);

    if(is_connect(e,ch))

    while(!esp_has_data_timeout(e->l,10000)) {
        output("no connection\n");
        ;
    }
    // fuck: if it's not a connect, what are you going to do?
    while(!is_connect(e, ch))
        ;
    return 1;
}

/*
 * default out-of-order message handler: ignore everything
 * before the connection is active.
 *
 * we should not be called after, so just panic if so.
 */
int default_handle_out_of_order_msg(esp_t *e) {
    if(!e->active_p)
        cmd_echo_line(e, "booting up: ignoring:");
    else
        lex_panic(e->l, "unexpected out of order message\n");
    return 1;
}

void handle_connect(esp_t *e, unsigned ch) {
    output("got a connect for %d\n", ch);
    e->open_conn[ch].active_p = 1;
    e->open_conn[ch].client_p = 0;
    e->open_conn[ch].type = ESP_TCP;
}
void handle_disconnect(esp_t *e, unsigned ch) {
    output("got a disconnect for %d\n", ch);
    e->open_conn[ch].active_p = 0;
}

// if you are running in server mode, will have to change this
// since you can receive connect and disconnect messages.   
// cleaner to use a function pointer, but we use flags since
// lab is short.
static int handle_ooo_server(esp_t *e) {
    unsigned ch;
    lex_panic(e->l, "in handle ooo server\n");
    if(is_connect(e,&ch)) {
        handle_connect(e,ch);
    } else if(is_disconnect(e, &ch)) {
        output("got a disconnect for %d\n", ch);
        handle_disconnect(e,ch);
    } else
        lex_panic(e->l, "unimplemented: you should handle other messages.\n");
    return 1;
}
static int handle_ooo_client(esp_t *e) {
    lex_panic(e->l, "client: AFAIK: should never receive unexpected messages.\n");
    return 1;
}

/********************************************************************
 * crude esp management.
 */

// is it up at all?  
int esp_is_up(esp_t *e) {
    at_cmd(e, "AT", "OK");
    return 1;
}

// drain random data that appears after a reset.  not sure a better way.
int at_drain(esp_t *e) {
    while(1) {
        if(match_line(e->l, "ready"))
            break;
        cmd_echo_line(e, "reset:");
    }
    // i seems like we have to wait awhile for the esp.
    esp_usleep(1000 * 1000);
    return 1;
}


// esp has various race conditions: we use delays to try to 
// allow it to finish what it is doing.
int at_delay(void) {
    esp_usleep(400*1000);
    return 1;
}

// this is our attempt to do a hard reset so that you don't have to 
// pull the esp in and out your computer.
// 
// NOTE: this is *NOT* guaranteed.  i experminted with this and that 
// (hence the seemingly contradictory actions and random delays).
// there is likely a better way.  so just use this as a starting point.
int esp_hard_reset(esp_t *e) {
    // we wait until no characters for 1ms.  otherwise we can get
    // random stuff from previous connections.   note: this 
    // is still a race condition (we can always get old stuff from a 
    // non-reset esp), so strictly speaking i think we need to
    // sit here in a tight loop issuing resets until things work ok.
    while(esp_has_data_timeout(e->l, 1000))
        lex_getc(e->l);
    debug("drained everything i think\n");
    if(!at_cmd_extra(e, "AT+RST", "OK") || !at_drain(e))
        panic("cannot reset??\n");

    // for these two, we can get random extraneous stuff during.
    return at_cmd_extra(e, "AT+RESTORE", "OK")
        && at_delay()
        && at_drain(e)
        // i think restore implies redone.
        && at_cmd_extra(e, "AT+RST", "OK")
        && at_drain(e)
        ;
}

/*********************************************************************
 * wifi connection routines.
 */

// AP server.
int esp_setup_wifi(esp_t *e) {
    assert(e->wifi && e->password);
    assert(!e->active_p);

    output("setup_wifie: on my esp this prints out: +CIFSR:APIP,\"%s\"\n", ESP_SERVER_IP);
    output("\tso 192.168.4.1 is the device's address\n");
    assert(strlen(e->password) >= 8);

    return 
           at_cmdv(e, "AT+CWSAP=\"%s\",\"%s\",5,3", e->wifi, e->password)
        && at_cmd(e, "AT+CWMODE=3", "OK")
        && at_cmd_extra(e, "AT+CIFSR", "OK")
        ;
}

int esp_connect_to_wifi(esp_t *e) {
    assert(e->wifi && e->password);
           // set mode client.
    return at_cmd_ok(e, "AT+CWMODE=1")
        // && at_cmd_rep_v(e, "WIFI CONNECTED", "AT+CWJAP=\"%s\",\"%s\"", e->wifi, e->password)
        // && at_cmdv(e, "AT+CWJAP=\"%s\",\"%s\"", e->wifi, e->password)
        && at_cmd_extrav(e, "AT+CWJAP=\"%s\",\"%s\"", e->wifi, e->password)
        ;
}

// you want to get a response, and then get an ok --- the way the code is 
// setup now, you can bail if you just get an ok without filling anything
// in.  is it better to not bundle everything together?
static unsigned 
issue_tcp_connect(esp_t *e, unsigned *ch, const char *server_ip, unsigned portn) {
    char cmd[1024];
    snprintf(cmd, sizeof cmd, "AT+CIPSTART=0,\"TCP\",\"192.168.4.1\",4444");

    // returns a uint.
    // make a run_cmd_uint
    cmd_puts(e, cmd);
    if(!cmd_ack(e, cmd, 0))
        panic("error issuing <%s>\n", cmd);
    // can't do this: need to do command_response 
    if(!cmd_responsef(e, 0, 0, "%u, CONNECT", ch))
        cmd_reject_line(e, "expecting <<uint>, CONNECT>\n");
    if(!cmd_response(e, "OK", 0, 0))
        cmd_reject_line(e, "did not receive ok!\n");
    debug("have a connect!  channel=%d\n", *ch);
    return 1;
}

/*********************************************************************
 * tcp routines.
 */
int esp_start_tcp_client(esp_t *e, const char *server_ip, unsigned portn) {
    assert(!e->active_p);

    // first we just do channel 0.
    unsigned ch = 0;

    // we are only connecting on a single channel.
    int result = 
        // enable multiple connections.
        at_cmd_ok(e,"AT+CIPMUX=1")
        // connect using the given port.
        // we can get a 0,CONNECT --i don't remember seeing this before.
        // we need to extract the channel from this.
#if 0
        && at_cmd_extrav(e,"AT+CIPSTART=0,\"TCP\",\"192.168.4.1\",4444");
#else
        && issue_tcp_connect(e, &ch, server_ip, portn)
#endif
        // && at_cmdv(e,"AT+CIPSTART=0,\"TCP\",\"%s\",%u", server_ip, portn)
        ;
    if(!result)
        return 0;

    assert(!ch);
    esp_conn_t *c = &e->open_conn[ch];
    assert(!c->active_p);
    c->client_p = 1;
    c->active_p = 1;
    c->type = ESP_TCP;
    c->server_ip = server_ip;
    c->portn = portn;

    e->handle_ooo_msg = handle_ooo_client;
    e->active_p = 1;
    return 1;
}

int esp_start_tcp_server(esp_t *e, unsigned portnum) {
    assert(!e->active_p);

    // output("connect using: <netcat 192.168.4.1 %d>\n", portnum);
    int result = at_cmd(e,"AT+CWMODE=3", "OK")        // dual mode
                && at_cmd(e,"AT+CIPMUX=1", "OK")

                // start a tcp server.
                // run using: netcat 192.168.4.1 <portnum>
                && at_cmdv(e,"AT+CIPSERVER=1,%u", portnum)

                // set timeout.
                && at_cmd_extra(e, "AT+CIPSTO?", "OK")
                && at_cmd_extra(e, "AT+CIPSTO=7200", "OK")
                && at_cmd_extra(e, "AT+CIPSTO?", "OK")

                && at_cmd_extra(e, "AT+CIFSR", "OK")
                && at_cmd_extra(e, "AT+GMR", "OK")
                ;
    if(!result)
        return 0;
    e->handle_ooo_msg = handle_ooo_server;
    e->active_p = 1;
    return 1;
}

#if 0
int esp_send(esp_t *e, unsigned ch, const void *data, unsigned n) {
    assert(n < ESP_MAX_PKT);

    char cmd[512];
    snprintf(cmd, sizeof cmd,  "AT+CIPSEND=%u,%u", ch, n);
    char ack[512];
    snprintf(ack, sizeof ack, "Recv %d bytes", n);

    // doc seems to indicate you do both?  or do you wait for an ack?
    cmd_puts(e, cmd);
    if(!esp_write_exact(e->l, data, n))
        panic("write exact failed\n");

    if(!cmd_ack(e, cmd, 1))
        lex_panic(e->l, "no ack for command: <%s>", cmd);
    if(!cmd_response(e, ack, 0, 0))
        lex_panic(e->l, "could not get command");

    if(!cmd_response(e, "SEND OK", 0, 0))
        lex_panic(e->l, "expected SEND OK");
    
#if 0
    if(!at_cmdv(fd, "AT+CIPSEND=%u,%u", ch, n))
        panic("send failed\n");
    if(!esp_write_exact(e, data, n))
        panic("write exact failed\n");
    if(!cmd_response(e, "SEND OK"
#endif
    return n;
}
#endif

// usually these will be received spontaneously.
int esp_is_recv(esp_t *e, unsigned *ch, void *data, unsigned maxbytes) {
    unsigned nbytes;
    if(!match(e->l, "+IPD,%u,%u:", ch, &nbytes))
        cmd_reject_line(e, "expected receive, have something else\n");

    esp_debug("got a recv: ch=%d, nbytes=%d, max=%d\n", *ch, nbytes,maxbytes);
    assert(nbytes>0);
    if(nbytes > maxbytes)
        lex_panic(e->l, "message is %d bytes long, we have %d space avail\n", nbytes, maxbytes);

    // this should be timeout based.
    uint8_t *m = data;
    int i;
    // DO NOT 0 terminate data!  will corrupt non-strings.
    for(i = 0; i < nbytes; i++)
        m[i] = lex_getc(e->l);
    esp_debug("received %d bytes, message=<%s>\n", nbytes, m);
    return nbytes;
}

int esp_send(esp_t *e, unsigned ch, const void *data, unsigned n) {
    assert(n>0);

    at_cmdv(e, "AT+CIPSEND=%u,%u", ch, n);
    // wait for a >, no EOL
    if(!match_tok(e->l, ">"))
        cmd_reject_line(e, "didn't match '>'");

    esp_debug("got an ack\n");
    // return error
    if(!esp_write_exact(e->l, data, n))
        panic("write failed\n");

    unsigned nsent;
    cmd_responsef(e, 0, 0, "Recv %u bytes", &nsent);
    if(n != nsent)
        panic("wanted to send %d bytes,  sent: %d\n", n, nsent);
    cmd_response(e, "SEND OK", 0, 0);
    return n;
}
