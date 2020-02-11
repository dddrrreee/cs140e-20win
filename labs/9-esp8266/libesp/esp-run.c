// engler, cs140e: issue at commands, handle the response.
#include <stdarg.h>
#include "libesp.h"

// if you want to print more, set to != 0.
static unsigned esp_verbose_p = 1;
void esp_set_verbose(int verbose_p) {
    esp_verbose_p = verbose_p;
}

/***********************************************************************
 * simple utility routines.
 */

// echo the current line out, printing <msg>
void cmd_echo_line(esp_t *e, const char *msg) {
    char buf[1024];
    if(!get_nonblank_line(e->l, buf, sizeof buf))
        return;
    output("\t%s: <%s>\n", msg, buf);
}

// can this fail?  i think has to be for OS reasons, not for esp, but am not sure.
static unsigned cmd_num = 1;
void cmd_puts(esp_t *e, const char *cmd_op) {
    char cmd[1024];

    unsigned n = strlen(cmd_op);
    assert(n);
    assert(n < sizeof cmd - 2);

    strcpy(cmd, cmd_op);
    esp_debug("%d: issuing exp-cmd <%s>\n", cmd_num, cmd);
    strcat(cmd, "\r\n");
    cmd_num++;

    if(!esp_write_exact(e->l, cmd, n+2))
        panic("write failed\n");
}

#if 0
void cmd_puts(esp_t *e, const char *fmt, ...) {
    char cmd[1024];
    va_list args;

    va_list ap;
    va_start(ap, fmt);
    if(vsnprintf(cmd, sizeof cmd - 2, fmt, ap)  < 0)
        panic("overflowed buffer <%s>\n", cmd);
    va_end(ap);
    output("%d: issuing exp-cmd <%s>\n", cmd_num, cmd);
    strcat(cmd, "\r\n");
    cmd_num++;

    unsigned n = strlen(cmd);
    assert(n);
    if(!esp_write_exact(e->l, cmd, n))
        panic("write failed\n");
}
#endif

/****************************************************************************
 * main routines to issue a command and wait for the expected response.
 */
int cmd_response(esp_t *e, const char *end, int can_fail_p, int extra_p) {
    lex_t *l = e->l;
    while(1) {
        // we matched what we wanted: done.
        if(match_line(l, end))
            break;

        // got an error: if we can fail, return.
        // otherwise die
        if(match_tok(l, "ERROR")) {
            if(can_fail_p)
                return 0;
            lex_panic(l, "fatal error: got an error\n");
        }

        // the rest checks for suspicious things.

#if 0
        // looks like something important but we were not waiting for it.
        if(match_tok(l, "OK")
        || match_tok(l, "SEND OK")
        || match_tok(l, "ready")
        || match_tok(l, "BUSY"))
            lex_panic(l, "suspicious end of command\n");
#endif

        // i don't think we should get a busy: possibly just sleep instead of panic?
        if(match_line(l, "busy s...") || match_line(l, "busy p...")) 
            lex_panic(l, "received a busy: sent too much I think (or too soon?)\n");

        // ok, is something else: if we can get extra stuff, skip it.
        // otherwise die.
        if(!extra_p)
            expect_blank_line(l);
        else
            cmd_echo_line(e, "skipping extra:");
    }
    // maybe echo as you go along?
    if(esp_verbose_p)
        lex_dump_str(l);
    return 1;
}

// returns when <cmd> has been acknowledged (echoed) by the esp
int cmd_ack(esp_t *e, const char *cmd, int extra_p) {
    /* 
     * from testing: if the esp has been up and we are simply restarting, at this point
     * we can we can get other messagses --- we need to process them.  the esp tells us
     * when it responds to *this* message by echoing the command back.  simple/cute. 
     * so we spin here until we see the echo, forwarding everything else to the 
     * messsage loop.
     */

    // the end of line is implicit in the match.
    while(!match_line(e->l, cmd)) {
        esp_debug("did not match <%s>:%d, going to do another message\n", cmd, cmd_num);
        if(!is_blank_line(e->l))
            e->handle_ooo_msg(e);
    }
    esp_debug("esp ack'd cmd <%s>:%d\n", cmd, cmd_num);
    return 1;
}

static int do_at_cmd(esp_t *e, const char *cmd, const char *response, int extra_p) {
    cmd_puts(e, cmd);
    if(!cmd_ack(e, cmd, extra_p))
        panic("error issuing cmd <%s>!\n", cmd);
    // cannot fail, no extra stuff.
    return cmd_response(e, response, 0, extra_p);
}

int at_cmd(esp_t *e, const char *cmd, const char *response) {
    return do_at_cmd(e, cmd, response, 0);
}
int at_cmd_extra(esp_t *e, const char *cmd, const char *response) {
    return do_at_cmd(e, cmd, response, 1);
}

#if 0
int at_cmd_extra(int fd, const char *at_cmd, const char *response) {
    return cmd_expect_helper(fd,at_cmd,response,1);
}

// We wait until no characters for 1ms.  Otherwise I think we can sometimes 
// get random stuff from previous connections.  This is still a race condition 
// (we can always get old stuff from a non-reset esp), so strictly speaking I 
// think we need to sit here in a tight loop issuing resets until things work ok.
// XXX: revisit.
void esp_eat_stale_data(lex_t *l) {
    while(esp_has_data_timeout(l,1000))
        lex_getc(l);
    debug("drained everything i think\n");
}

// from what i can tell, this is a pretty robust way to reset the esp.
void esp_reset(lex_t *l) {
    esp_eat_stale_data(l);
}
#endif
