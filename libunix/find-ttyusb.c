// engler, cs140e.
// alot of code duplication.   feel free to remove!
#include <assert.h>
#include <fcntl.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include "libunix.h"

#define _SVID_SOURCE
#include <dirent.h>
static const char *ttyusb_prefixes[] = {
    "ttyUSB",    // linux
    "cu.SLAB_USB", // mac os
    0
};

static int filter(const struct dirent *d) {
    for(const char **p = ttyusb_prefixes; *p; p++)
        if(strncmp(*p,d->d_name, strlen(*p)) == 0)
            return 1;
    return 0;
}

static struct dirent **find_ttyusb_helper(int *n) {
    struct dirent **namelist;
    *n = scandir("/dev", &namelist, filter, alphasort);
    if(*n < 0)
        sys_die(scandir, "scandir failed: impossilbe?");
    if(*n == 0)
        panic("did not find any tty in /dev\n");
    return namelist;
}

// find the TTY-usb device (if any) by using <scandir> to search for
// a device with a prefix given by <ttyusb_prefixes> in /dev
// returns:
//  - device name.
// panic's if 0 or more than 1.
//
char *find_ttyusb(void) {
    int n;
    struct dirent **namelist = find_ttyusb_helper(&n);
    if(n > 1) {
        output("found more than one tty?\n");
        while (n-- > 0)
            output("%s\n", namelist[n]->d_name);
        exit(1);
    }
    char *p =  strcatf("/dev/%s", namelist[0]->d_name);
    debug("FOUND: <%s>\n", p);
    return p;
}

static unsigned get_modtime(const char *name) {
    struct stat s;
    char buf[1024];
    sprintf(buf, "/dev/%s", name);
    if(stat(buf, &s) < 0)
        sys_die(stat, "died on <%s>\n", buf);
    return s.st_ctime;
}

char *find_ttyusb_last(void) {
    int n;
    struct dirent **namelist = find_ttyusb_helper(&n);
    assert(n>0);

    struct dirent *last = namelist[0];
    if(n > 0) {
        unsigned max_mod = get_modtime(last->d_name);
        for(int i  = 0; i < n; i++) {
            unsigned mod = get_modtime(namelist[i]->d_name);
            if(mod > max_mod) {
                max_mod = mod;
                last = namelist[i];
            }
        }
    }
    char *p =  strcatf("/dev/%s", last->d_name);
    debug("FOUND: <%s>\n", p);
    return p;
}

char *find_ttyusb_first(void) {
    int n;
    struct dirent **namelist = find_ttyusb_helper(&n);
    assert(n>0);

    struct dirent *first = namelist[0];
    if(n > 0) {
        unsigned min_mod = get_modtime(first->d_name);
        for(int i  = 1; i < n; i++) {
            unsigned mod = get_modtime(namelist[i]->d_name);
            if(mod < min_mod) {
                min_mod = mod;
                first = namelist[i];
            }
        }
    }
    char *p =  strcatf("/dev/%s", first->d_name);
    debug("FOUND: <%s>\n", p);
    return p;
}

