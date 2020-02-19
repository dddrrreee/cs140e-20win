// engler, cs140: from Frank Hofmann's fuse talk, added comments.
//
// FUSE: Filesystem in Userspace
// Copyright (C) 2001-2005 Miklos Szeredi <miklos@szeredi.hu>
// This program can be distributed under the terms of the GNU GPL.
// See the file COPYING.
#define FUSE_USE_VERSION 26
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "demand.h"


static const char *hello_str = "hello cs140e: today let us do a short lab!\n", *hello_path = "/hello";

static int hello_open(const char *path, struct fuse_file_info *fi) {
    // we only have a single file.
    if(strcmp(path, hello_path) != 0)
        return -ENOENT;

    // we only allow read-only access to the file.
    // Q: what happens if we remove this check?
    if((fi->flags & 3) != O_RDONLY)
        return -EACCES;

    // 0 = success 
    return 0;
}

// list out the contents of the directory pointed to by <path>.  in this
// case we only have a single file and no subdirectories, so always
// return { ".", "..", "hello" }
//
// NOTE: filler can fail, but we assume it will not.
static int hello_readdir(const char *path, void *buf, 
            fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {

    if(strcmp(path, "/") != 0)
        return -ENOENT;
    // called to add entries to the caller. 
    // Q: what happens when you leave off each one?
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, hello_path + 1, NULL, 0);
    return 0;
}

static int hello_getattr(const char *path, struct stat *stbuf) {
    memset(stbuf, 0, sizeof(struct stat));
    int res = 0;

    // getattr("/"): is a directory, and give the permissions:
    //  0755:
    //      0700: S_IXWXU: file owner has read,write and execute perm
    //      0050: group can read, can execute
    //      0005: other can read can execute
    // Q: what happens if you give it different permissions?
    if(strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    // getattr("/hello") is our file: all entities only have
    // read permissions 
    //  - user  (0400), 
    //  - group (0040),
    //  - other (0004)
    } else if(strcmp(path, hello_path) == 0) {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = strlen(hello_str);
    } else
        res = -ENOENT;

    return res;
}

#define XMIN(x,y) ((x)<(y) ? (x) : (y))

static int hello_read(const char *path, char *buf, size_t size, 
                    off_t offset, struct fuse_file_info *fi) {

    size_t len = strlen(hello_str);
    // same pattern: fail if they are looking for a different file
    // than /hello
    if(strcmp(path, hello_path) != 0)
        return -ENOENT;
    if (offset > len)
        return 0;

    // note: we are not null terminating.
    //
    // Q: what happens if we keep returning different
    // values?
    size = XMIN(size, len - offset);
    memcpy(buf, hello_str + offset, size);
    return size;
}

// other entries will be set to 0.
static struct fuse_operations hello_oper = {
    .getattr = hello_getattr, 
    .readdir = hello_readdir,
    .open = hello_open, 
    .read = hello_read,
};

// mkdir /tmp/fuse
// ./hello /tmp/fuse
int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &hello_oper, 0);
}
