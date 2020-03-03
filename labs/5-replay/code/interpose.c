#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <sys/wait.h>

#include <sys/stat.h>


#include "libunix.h"

#include "interpose.h"

static int handle_child_exit(int pid, int can_fail_p);

/********************************************************************************
 * some starter code.   not-impressive attempt to be OO about the endpoints (pi or
 * unix) 

 * should be extended to do for logs as well, and to actually use the 
 * failure_fp to check endpoints for problems.  it calls the helper code you
 * have to implement.
 */
static const char *type_name(int ty) {
    if(ty == PI_OUTPUT)
        return "pi side";
    if(ty == UNIX_OUTPUT)
        return "unix side";
    panic("invalid type %d\n", ty);
}

// simple endpoint.  name of process, its pid, the socket fd we use to 
// talk to/fro with.
endpt_t endpt_mk(int ty, const char *dev_name, int fd, fail_ptr_t fp) {
    return (endpt_t) { 
                .type = ty, 
                .name = type_name(ty),
                .dev_name = dev_name, 
                .fd = fd, 
                .failure = fp 
    };
}


// we use a macro so we can print the file/func/linenumber of the failure.
#define end_failed(e, cmd, args...)  do {   \
    debug(args);                            \
    e->failure(e,cmd);                      \
} while(0)
    
// not used at the moment: good extension to put in.
static void pi_end_failed(endpt_t *e, const char *cmd) {
    output("pi side failed when issuing <%s>\n", cmd);
    if(!tty_gone(e->dev_name))
        panic("I/O error on serial dev <%s>, but it wasn't unplugged\n", e->dev_name);
    output("going to exit\n");
    exit(0);
}


// unused at the moment: should extend.
static void unix_end_failed(endpt_t *e, const char *cmd) {
    output("unix side failed when issuing <%s>\n", cmd);
    if(!handle_child_exit(e->pid,0))
        panic("unix side I/O error with child still alive.\n");
}

// fork the unix side bootloader code and return a file descriptor 
// for communicating with the client.
//   1. create a pty and get fd.
//   2. fork/exec the client code.
//   3. create a watchdog to kill the child if the parent dies.
//   4. return descriptor.
endpt_t start_unix_side(char *prog_name, char **argv_rest) {
#if 1
#   define MAX_ARGS 12
    char * argv[MAX_ARGS];
    memset(argv, 0, sizeof argv);

    argv[0] = prog_name;
    argv[1] = "/tmp/fake-dev";
    for(int i = 0; argv_rest[i]; i++) {
        assert((i+2)<MAX_ARGS);
        argv[i+2] = argv_rest[i];
    }
#else

    char * argv[4];
    argv[0] = prog_name;
    // so it does not go search for one.
    argv[1] = "/tmp/fake-dev";
    argv[2] = pi_prog_name;
    argv[3] = 0;
#endif

    int pid;
    #   define TRACE_FD 21
    // you implement exec_server_socket.
    int fd = exec_server_socket(&pid, argv, TRACE_FD);
    output("pi-install pid = %d\n", pid);

    endpt_t e = endpt_mk(UNIX_OUTPUT, argv[1], fd, unix_end_failed);
    e.pid = pid;
    return e;
}
    


// not much to do: we just open it.
endpt_t start_pi_side(const char *dev_name) {
    assert(dev_name);
    int fd = open_tty(dev_name);
    fd = set_tty_to_8n1(fd, B115200, 1);
    return endpt_mk(PI_OUTPUT, dev_name, fd, pi_end_failed);
}

// we depend on a specific log format, so don't modify please.
static void wr_log_byte(int fd, uint8_t ty, uint8_t b) {
    char buf[1024];
    sprintf(buf, "%c,0x%.2x\n", ty,b);
    write_exact(fd, buf, strlen(buf));
}

// flip <c> to some other value that is not <c>
static uint8_t corrupt_byte(uint8_t c) {
    uint8_t v;
    do { 
        v = random();
    } while(v == c);
    return v;
}

/*******************************************************************************
 * your code goes below.
 */

#include <errno.h>

// returns 0 if not exited, 1 if exited cleanly, < 0 otherwise.
// should clean this up.
static int handle_child_exit(int pid, int can_fail_p) {
	int status;
	int r;
	usleep(1000);
   
	r = waitpid(pid, &status, WNOHANG);
	
	if (r < 0) {
		printf("Unix did not exit cleanly\n");
	} else if (r == 0){
		printf("Unix has not stopped yet\n");
		return 0;
	} else {
		;
	}
	
	if(WIFEXITED(status)) {
		if(!WEXITSTATUS(status)) { 
			printf("Unix exited cleanly with status %d\n", WEXITSTATUS(status));
			return 1;
		} else {
			printf("Unix did not exit cleanly with status %d\n", WEXITSTATUS(status));
			return -1;
		}
	}

	printf("Finally Unix did not exit cleanly with status %d\n", WEXITSTATUS(status));
	return -1;

}

static int forward(endpt_t *in, endpt_t *out) {
    char buf[1024];
    int n;
	n = read(in->fd, buf, sizeof(buf));
    write(out->fd, buf, n);
	for(int i = 0; i < n; i++)
        wr_log_byte(in->log_fd, in->type, buf[i]);
    return n;
}

// do you really want to expose all of this stuff?
void trace(int log_fd, endpt_t *u, endpt_t *p) {
    assert(is_unix_side(u));
    assert(!is_unix_side(p));

    u->log_fd = p->log_fd = log_fd;
    assert(p->fd > 0);
    assert(u->fd > 0);
    assert(u->pid);

    while(1) {
        // forward data from one file descriptor to another.
        while(can_read(p->fd)) {
            output("reading from pi\n");
            if(!forward(p, u))
                panic("pi failed??\n");
        }
        while(can_read(u->fd)) {
            output("reading from unix\n");
            if(!forward(u, p))
                goto done;
        }
    }

done:
    if(!handle_child_exit(u->pid,0))
        panic("received EOF: unix side should be dead!\n");
    else
        output("unix side is dead\n");
}

int read_exact_can_fail2(int fd, void *data, unsigned n) {
    assert(n);
    int got;
    if((got = read(fd, data, n)) < 0)
        sys_die(read, read_exact failed);
    if(got != n) {
        debug("expected a read of %d bytes, got %d\n", n, got);
        return 0;
    }
    return n;
} 

static int read_exact_fail(int fd, void *data, unsigned n, int can_fail_p) {
	int status;
	if(can_fail_p) {
		status = read_exact_can_fail2(fd, data, n);
	} else {
		status = read_exact(fd, data, n);
	}
	return status;
}

void close_open_fds_except(int fd){
	struct rlimit l;

	no_fail(getrlimit(RLIMIT_NOFILE, &l));
	int i;
	for(i = 3; i < l.rlim_cur; i++) {
		if (i != fd) {
			close(i);
		}
	}
}

void close_open_fds(void) {
	close_open_fds_except(-1);
}


// should give it a log to emit into, but.
int replay_unix(endpt_t *u, log_ent_t *log, unsigned n, int fail_i) {
    assert(is_unix_side(u));
    assert(u->pid);

    int corrupted_p = 0;
    int sent_bytes = 0;

    uint8_t c;
	int status;

    for(int i = 0; i < n; i++) {
        log_ent_t *e = &log[i];

        if(e->sender == PI_OUTPUT) {
            c = e->v;
			//output("Sender %hhu, payload; %d\n", e->sender, e->v);
            if(fail_i == sent_bytes) {
                c = corrupt_byte(c);
                output("CORRUPTED byte %d\n", fail_i);
                corrupted_p = 1;
            }
            write_exact(u->fd, &c, 1);
            //output("In PI\n");
            sent_bytes++;
        } else {
			uint8_t b; 
			c = e->v;
			// If endpoint sender is Unix
			// Check if we can read:
			// If not, then waitpid
			// If yes, then read and see if we got something out
			if(!can_read(u->fd)) {
				if(1 == handle_child_exit(u->pid, corrupted_p)) {
					output("Handling child exit\n");
					return corrupted_p;
				} 
			 } else {
			 	if(1 == read_exact_fail(u->fd, &b, 1, corrupted_p)) {
					;
				} else {
					goto replay_done;
				}
			 }
		}
	}
    
	// have to spin for some number of iterations checking for child 
    // exit.
replay_done:
    for(int i = 0; i < 10; i++) {
        if(handle_child_exit(u->pid, corrupted_p))
            return corrupted_p;
        usleep(1000);
    }
    die("ERROR: ran out of trace and unix is not dead\n");
}
