
#include <assert.h>
#include <fcntl.h>           /* Definition of AT_* constants */
#include <string.h> 
#include <sys/stat.h>

#define _SVID_SOURCE
#include <dirent.h>

#include "libunix.h"

static int max_test_n = 0;

// static const char *prefix = "log-file-";
static const char *log_prefix = 0;

// looking for the maxium one
static int filter(const struct dirent *d) {
    assert(log_prefix);
    int n = strlen(log_prefix);

    if(strncmp(log_prefix,d->d_name, n) != 0)
        return 0;
    int test_n = atoi(d->d_name+n);
    if(max_test_n < test_n)
        max_test_n = test_n;
    return 1;
}

// look through the logdir for the highest log file that matches <prefix>.
//  create the next largest one.
//
// returns: the fd for the open file and the file name in <name>.
//
// XXX: probably should extend so they can pass a function in.
int log_create_next(const char *log_dir, const char *prefix, char *name) {
    log_prefix = prefix;

    struct dirent **namelist;
    int n;
    max_test_n  = 0;
    n = scandir(log_dir, &namelist, filter, alphasort);
    if(n < 0) {
        sys_debug(scandir, "scandir: can't find the test directory");
        if(mkdir(log_dir, 0700) < 0)
            sys_die(mkdir, mkdir failed: weird);
        n = scandir(log_dir, &namelist, filter, alphasort);
        if(n < 0)
            panic("failed to create directory <%s>\n", log_dir);
        demand(n == 0, directory cannot have entries!);
    } else if(n == 0)
        debug("trace: did not find any tests in test dir <%s>\n", log_dir);
    else
        max_test_n++;

    sprintf(name, "%s/%s%d.txt", log_dir, prefix, max_test_n);
    int fd = open(name, O_RDWR | O_CREAT, S_IRWXU);
    if(fd < 0)
        sys_die(open, "failed could not open <%s>\n", name);
    debug("opened file fd=%d, <%s>\n", fd, name);
    log_prefix = 0;
    return fd;
}

// match all dirents whose names match <log_prefix>
static int filter_tests(const struct dirent *d) {
    int n = strlen(log_prefix);
    return strncmp(log_prefix,d->d_name, n) == 0;
}

// returns array pointing to all file names in <dirname> that match <prefix>
char **log_list_all(const char *dirname, const char *prefix) {
    log_prefix = prefix;

    // otherwise search in /dev for a tty device.
    struct dirent **namelist;
    int n = scandir(dirname, &namelist, filter_tests, alphasort);
    if(n < 0)
        panic("scandir: can't find the test directory: <%s>", dirname);
    if(n == 0)
        panic("scandir: directory <%s> has no entries?", dirname);

    char **log_list = calloc(n+1, sizeof log_list[0]);
    for(int i = 0; i < n; i++) {
        char buf[1024];
        sprintf(buf, "%s/%s", dirname, namelist[i]->d_name);

        log_list[i] = strdup(buf);
    }
    log_prefix = 0;
    return log_list;
}
