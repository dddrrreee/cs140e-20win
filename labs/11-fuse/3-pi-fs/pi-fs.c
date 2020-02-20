/* 
 * engler, cs140e:  skeleton for the pi-fs.  you are more than welcome to just take your
 * 0-hello code and start from there.  we've provided this to try to help a little.
 */

#define FUSE_USE_VERSION 26
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include "libunix.h"

/****************************************************************************
 * Fuse methods.  use can use this approach or do your own.  
 */

struct dirent;
typedef int (*on_write_fp)(struct dirent *e, const char *path, const char *buf,
                            size_t size, off_t offset, void *data);

// 0 = skip write to base file.  1 = do it.
typedef int (*on_read_fp)(struct dirent *e, const char *path, char *buf,
                            size_t size, off_t offset, void *data);

/**********************************************************************
 * unix permissions. for examples:
 *      - "man 2 stat"
 *      - "man 2 open"
 */
typedef enum {
    perm_rd = 0444,
    perm_wr = 0222,
    // not sure if we use this.
    perm_exec = 0111,
    perm_rw = perm_rd | perm_wr | perm_exec,
} perm_t;



// this way of creating a ramFS is not the right way to do things.
// we use expedient hacks so we can strip the things down for a 
// couple hour lab --- you'd really want a real tree (or perhaps DAG) 
// that mirrors the fake file system you are making.  unfortunately,
// if we gave that as starter code, you'd have to spend a bunch of 
// time figuring it out.  and if we didn't, you'd have to write it.
// i'd rather you spend time in lab on figuring out FUSE.
typedef struct pi_file {
    char *data;
    size_t n_alloc,    // total bytes allocated.
             n_data;     // how many bytes of data
} file_t;

// for first lab: no directories.
typedef struct dirent {
    char *name;
    int flags;
    file_t *f;

    // allow user to extend each file/dir with actions.
    on_write_fp on_wr;
    on_read_fp on_rd;
} dirent_t;

static int do_reboot(dirent_t *e, const char *path, const char *buf,
                            size_t size, off_t offset, void *data) {
    unimplemented();
}

static int do_echo(dirent_t *e, const char *path, const char *buf,
                            size_t size, off_t offset, void *data) {
    unimplemented();
}

static int do_run(dirent_t *e, const char *path, const char *buf,
                            size_t size, off_t offset, void *data) {
    unimplemented();
}

// simple pi file system: no directories.
static dirent_t root[] = {
    { .name = "/echo.cmd", perm_rw, 0, .on_wr = do_echo },
    { .name = "/reboot.cmd", perm_rw, 0, .on_wr = do_reboot},
    { .name = "/run.cmd", perm_rw, 0, .on_wr = do_run},
    { .name = "/console", perm_rd, 0 },
    { 0 }
};

static int pi_open(const char *path, struct fuse_file_info *fi) {
    int retv;
    debug("opening\n");
    unimplemented();
    return retv;
}

static int pi_readdir(const char *path, void *buf, fuse_fill_dir_t filler, 
                            off_t offset, struct fuse_file_info *fi) {
    debug("readdir\n");
    if(strcmp(path, "/") != 0)
        return -ENOENT;
    unimplemented();
    return 0;
}

static int pi_getattr(const char *path, struct stat *stbuf) {
    debug("getattr");
    unimplemented();
    return 0;
}

static int pi_read(const char *path, char *buf, size_t size, 
                    off_t offset, struct fuse_file_info *fi) {

    debug("read");
    unimplemented();
    return size;
}

static int pi_write(const char *path, const char *buf, size_t size, 
        off_t offset, struct fuse_file_info *fi) {

    debug("read");
    unimplemented();
    return size;
}

/** Change the size of a file */
static int pi_truncate(const char *path, off_t length) {
    debug("truncate");
    unimplemented();
    return 0;
}

static int 
pi_ftruncate(const char *path, off_t offset, struct fuse_file_info *fi) {
    return pi_truncate(path, offset);
}

/* these were enough for me for the lab. */
static struct fuse_operations pi_oper = {
    .getattr = pi_getattr, 
    .readdir = pi_readdir,
    .open = pi_open, 
    .read = pi_read,
    .write = pi_write,
    .truncate = pi_truncate,
    .ftruncate = pi_ftruncate,
};

/*
    -d Enable debugging output (implies -f).

    -f Run in foreground; this is useful if you're running under a
    debugger. WARNING: When -f is given, Fuse's working directory is
    the directory you were in when you started it. Without -f, Fuse
    changes directories to "/". This will screw you up if you use
    relative pathnames.

    -s Run single-threaded instead of multi-threaded. 
*/
int main(int argc, char *argv[]) {
#if 0
    // make sure you can look these up using whatever method you have!
    assert(ent_lookup(root, "/console"));
    assert(ent_lookup(root, "/echo.cmd"));
    assert(ent_lookup(root, "/reboot.cmd"));
    assert(ent_lookup(root, "/run.cmd"));
#endif
    if(!fd_is_open(HANDOFF_FD))
        panic("pi-fs: must run with <HANDOFF_FD> as an open file descriptor\n");

    return fuse_main(argc, argv, &pi_oper, 0);
}
