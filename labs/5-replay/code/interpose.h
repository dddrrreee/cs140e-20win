#ifndef __INTERPOSE_H__
#define __INTERPOSE_H__

#include <stdint.h>
struct endpoint;
typedef void (*fail_ptr_t)(struct endpoint *self, const char *msg);

enum { PI_OUTPUT = 'p', UNIX_OUTPUT='u' };

typedef struct endpoint {
#   define is_unix_side(e) ((e)->type == UNIX_OUTPUT)
    int type;
    const char *name;
    const char *dev_name;   // name of the device we interface through.
    int fd;                 // device fd.
    int log_fd;

    pid_t pid;              // its pid (if any)
    fail_ptr_t failure;     // called on failure.
} endpt_t;

endpt_t start_pi_side(const char *dev_name);
endpt_t start_unix_side(char *prog_name, char *pi_prog_name);


typedef struct {
    uint8_t sender;
    uint8_t v;
} log_ent_t;

void trace(int log_fd, endpt_t *u, endpt_t *p);
int replay_unix(endpt_t *u, log_ent_t *log, unsigned n, int fail_i);

log_ent_t *log_parse_tuples(const char *name, unsigned *n);

#endif
