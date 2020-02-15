/*
 * engler, cs140e: routines to use the ESP.  put all your code here
 * and in its header (esp-commands.h).  do not modify anything else 
 * in case we need to push bug fixes.  sorry.
 */
#include <stdarg.h>
#include "libesp.h"

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

// if you are running in server mode, will have to change this
// since you can receive connect and disconnect messages.   
// cleaner to use a function pointer, but we use flags since
// lab is short.
static int handle_ooo_server(esp_t *e) {
    //lex_panic(e->l, "unimplemented: you should handle other messages.\n");
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
        // should only get an OK
           at_cmdv(e, "AT+CWSAP=\"%s\",\"%s\",5,3", e->wifi, e->password)
        // should only get an OK
        && at_cmd(e, "AT+CWMODE=3", "OK")
        // prints out extra stuff
        && at_cmd_extra(e, "AT+CIFSR", "OK")
        ;
}

int esp_connect_to_wifi(esp_t *e) {
    assert(e->wifi && e->password);
    // double init
    assert(!e->active_p);
    at_cmdv(e, "AT+CWMODE=3");
	at_cmdv(e, "AT+CWJAP=%s,%s", e->ssid, e->password);
	//unimplemented();
	//return at_cmdv(e, "AT+CIPSTART=\"%s\", \"%s\", \"%d\" ", "TCP", e->wifi, 8080); 
	
}


/*********************************************************************
 * tcp routines.
 */
int esp_start_tcp_client(esp_t *e, const char *server_ip, unsigned portn) {
    assert(!e->active_p);

/*
        at_cmdv(e, "AT+CWSAP=\"%s\",\"%s\",5,3", e->wifi, e->password)
        // should only get an OK
        && at_cmd(e, "AT+CWMODE=3", "OK")
        // prints out extra stuff
        && at_cmd_extra(e, "AT+CIFSR", "OK")

		&& at_cmd(e, "AT+CIPSERVER=1", "OK");
*/


    e->handle_ooo_msg = handle_ooo_client;
    e->active_p = 1;
    return 1;
}

int esp_start_tcp_server(esp_t *e, unsigned portnum) {
    assert(!e->active_p);
    
	at_cmd(e, "AT+CIPMUX=1", "OK");

	at_cmdv(e, "AT+CIPSERVER=1,%u", portnum);

    
#if 0
    I would also do the following after you set up the server so that
    you can control the timeout.
   
    // set timeout.
    && at_cmd_extra(e, "AT+CIPSTO?", "OK")
    && at_cmd_extra(e, "AT+CIPSTO=7200", "OK")
    && at_cmd_extra(e, "AT+CIPSTO?", "OK")

    && at_cmd_extra(e, "AT+CIFSR", "OK")
    && at_cmd_extra(e, "AT+GMR", "OK")
    ;
#endif



    e->handle_ooo_msg = handle_ooo_server;
    e->active_p = 1;
    return 1;
}

int esp_connect(esp_t *e) {
    unsigned ch = 0;
	char buf [4096];
	char buf_cmd [4096];
	char buf_rep [4096]; 

	int status = esp_read(e->l, buf, 4096, 314159);
	
	if(status < 0) {
		printf("Invalid\n");
		return -1;
	} else if (status == 0) {
		printf("ESP timed out\n");
		return 0;
	} else {
		while(status <= 1) {
			status += esp_read(e->l, buf_cmd, 4096, 314159);
		}
		ch = status;
		printf("Status: %d\n", status);
		printf("Buf:%s\n", buf);

		int i;
		for(i = 0; i < status; i++) {
			if(buf_cmd[i] == ','){
				buf_cmd[i] = ' ';
			}
		}

		int j = 0;
		for (i = 0; i < status; i++){
			if(buf_cmd[i] == ' ') {
				;
			} else {
				buf_rep[j++] = buf_cmd[i];
			}
		}
		printf("Buf:%s\n", buf_rep);
	}
	
	return buf[0];
}

int esp_send(esp_t *e, unsigned ch, const void *data, unsigned n) {
    assert(n < ESP_MAX_PKT);
    
	at_cmdv(e, "AT+CIPSEND=%u,%u", ch, n); 

	match_tok(e->l, ">");

	esp_write_exact(e->l, data, n);
	
	match_line(e->l, "Recv %u bytes", &n);
	
	match_line(e->l, "SEND OK");

    return n;
}

// return bytes received.
int esp_recv(esp_t *e, unsigned ch, void *data, unsigned n) {
	// create char buffer
	// get non-blank lien e->l, 
	
	unimplemented();
}

