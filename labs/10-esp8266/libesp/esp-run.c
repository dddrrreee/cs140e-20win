// engler, cs140e: issue at commands, handle the response.
#include <stdarg.h>
#include "libesp.h"

// if you want to print more, set to != 0.
unsigned esp_verbose_p = 1;
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

#if 0
void cmd_reject_line(esp_t *e, const char *msg) {
    cmd_echo_line(e,msg);
    panic("fatal error\n");
}
#endif

#if 0
void reject_line(lex_t *l, const char *msg) {
    char buf[1024];
    if(!get_nonblank_line(l, buf, sizeof buf))
        panic("impossible non-blank\n"); 
    output("%s: rejecting line: <%s>\n", msg, buf);
}
#endif



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

#define esp_die(_e, fmt, args...)  do { \
            char buf[1024];                                 \
            if(!get_nonblank_line(l, buf, sizeof buf))          \
                panic("impossible: expected non-blank!\n"); \
            lex_panic(e->l, fmt, args);     \
} while(0)

/****************************************************************************
 * main routines to issue a command and wait for the expected response.
 * we might have to pass in a vector of allowed responses: some commands
 * have a variety.
 */
int cmd_responsev(esp_t *e, int can_fail_p, int extra_p, const char *fmt, va_list arg) {
    lex_t *l = e->l;
    while(1) {
        // we matched what we wanted: done.
        if(match_linev(l, fmt, arg))
            break;
#if 0
        // no-op
        if(match_line(l, "no change")
        || match_line(l, "OK"))
            break;
#endif

#if 0
        if(match_line(l, "WIFI GOT IP")
        || match_line(l, "WIFI CONNECTED"))
            continue;
#endif

        // got an error: if we can fail, return.
        // otherwise die
        if(match_tok(l, "ERROR")) {
            if(can_fail_p)
                return 0;
            lex_panic(l, "fatal error: got an error\n");
        }

        // the rest checks for suspicious things.
        // looks like something important but we were not waiting for it.
        if(match_tok(l, "OK")
        || match_tok(l, "SEND OK")
        || match_tok(l, "ready")
        || match_tok(l, "BUSY"))
            lex_panic(l, "suspicious end of command\n");

        // i don't think we should get a busy: possibly just sleep instead of panic?
        if(match_line(l, "busy s...") || match_line(l, "busy p...")) {
            esp_debug("received a busy: sent too much I think (or too soon?)\n");
            esp_debug("waiting for: <%s>\n", fmt);
            esp_usleep(20 * 1000);
            continue;
        }

        // ok, is something else: if we can get extra stuff, skip it.
        // otherwise die.
        if(extra_p)
            cmd_echo_line(e, "skipping extra:");
        else if(!match_blank_line(l)) {
            char buf[1024];
            if(!get_nonblank_line(l, buf, sizeof buf))
                panic("expected non-blank!\n");
            lex_panic(e->l, "specified no extra output for command.  hit some:<%s>.\n", buf);
        }
    }
    // maybe echo as you go along?
#if 0
    if(esp_verbose_p)
        lex_dump_str(l);
#endif
    return 1;
}

int cmd_responsef(esp_t *e, int can_fail_p, int extra_p, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int res = cmd_responsev(e, can_fail_p, extra_p, fmt, args);
    va_end(args);
    return res;
}

static int match_busy(lex_t *l) {
    return match_line(l, "busy s...") || match_line(l, "busy p..."); 
}


int cmd_response(esp_t *e, const char *end, int can_fail_p, int extra_p) {
    lex_t *l = e->l;
    while(1) {
        // we matched what we wanted: done.
        if(match_line(l, end))
            break;

#if 0
        // no-op
        if(match_line(l, "no change")
        || match_line(l, "OK"))
            break;
#endif

#if 0
        if(match_line(l, "WIFI GOT IP")
        || match_line(l, "WIFI CONNECTED"))
            continue;
#endif

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

        if(match_busy(l)) {
            esp_debug("received busy while waiting for: <%s>\n", end);
            esp_usleep(20 * 1000);
            continue;
        }


        // ok, is something else: if we can get extra stuff, skip it.
        // otherwise die.
        if(extra_p)
            cmd_echo_line(e, "skipping extra:");
        else if(!match_blank_line(l)) {
            char buf[1024];
            if(!get_nonblank_line(l, buf, sizeof buf))
                panic("expected non-blank!\n");
            lex_panic(e->l, "specified no extra output for command.  hit some:<%s>.\n", buf);
        }
    }
    // maybe echo as you go along?
#if 0
    if(esp_verbose_p)
        lex_dump_str(l);
#endif
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
        if(match_blank_line(e->l))
            continue;
        if(match_busy(e->l)) {
            debug("received a busy while waitin for ack of <%s>\n", cmd);
            esp_usleep(20*1000);
            continue;
        }
        // cmd_reject_line(e,"did not match <%s>:%d, going to do another message\n", cmd, cmd_num);
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
