#ifndef __ESP_RUN_H__
#define __ESP_RUN_H__

// 1. issue command
void cmd_puts(esp_t *e, const char *cmd);
#define cmd_putsv(e, fmt, args...) ({                   \
    char buf[512];                                      \
    if(snprintf(buf, sizeof buf, fmt, ##args)  < 0)     \
        panic("overflowed buffer <%s>\n", buf);         \
    cmd_puts(e,buf);                                    \
})


// 2. returns when <cmd> has been acknowledged (echoed) by the esp
int cmd_ack(esp_t *e, const char *cmd, int extra_p);

// 3. handle response (OK, ERROR, etc)
int cmd_response(esp_t *e, const char *end, int can_fail_p, int extra_p);


// bundles 1,2,3 above.
int at_cmd(esp_t *e, const char *cmd, const char *response);

// assumes "OK" is correct response.
static inline int at_cmd_ok(esp_t *e, const char *cmd) {
    return at_cmd(e,cmd,"OK");
}

// allow a printf-format string for cmd
#define at_cmdv(e, fmt, args...) ({                     \
    char buf[512];                                      \
    if(snprintf(buf, sizeof buf, fmt, ##args)  < 0)     \
        panic("overflowed buffer <%s>\n", buf);         \
    at_cmd_ok(e,buf);                                   \
})

// bundles the above: echoes anything extra.
int at_cmd_extra(esp_t *e, const char *cmd, const char *response);

// bundles the above, runs <fn(lex_t *l, data)> on any unmatched lines.
typedef int (*at_cmd_fn_t)(esp_t *e, void *data);
int at_cmd_extra_fn(esp_t  *e, const char *cmd, 
            const char *response, at_cmd_fn_t fn, void *data);

// called by at_cmd_*, defined by the client in esp-commands.c
void esp_handle_out_of_order_msg(esp_t *e);

// prints and consumes the current line.
void cmd_echo_line(esp_t *e, const char *msg);

#endif
