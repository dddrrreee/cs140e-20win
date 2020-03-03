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

#include "rpi.h"
#include "pi-message.h"

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


static int using_pi_p;

void put_byte(int fd, uint8_t b) {
	int r;
	if((r = write(fd, &b, 1)) != 1) {
		sys_die(write, "Write failed");
	}
}

enum { pi_fd = HANDOFF_FD };

static void put_uint8(uint8_t b)   { write_exact(pi_fd, &b, 1); }
static void put_uint32(uint32_t u) { write_exact(pi_fd, &u, 4); }

static uint8_t get_uint8(void) {
    uint8_t b;
    read_exact(pi_fd, &b, 1);
    return b;
}
static uint32_t get_uint32(void) {
    uint32_t u;
    read_exact(pi_fd, &u, 4);
    return u;
}

// *ALWAYS* call <get_op> to parse an op: checks if it is a PI_PRINT_CHAR
// and emits if so, otherwise returns.
//
// you could use an extension like this to handle all
// opcodes that can be spontaneously sent by the pi code.
//
// or, if you use threads have a thread waiting on the pi fd and pushing
// output to a circular buf for the consumer thread.
uint8_t get_op(void) {
    uint8_t op;

    while(1) {
        op = get_uint8();
        if(op != PI_PRINT_CHAR)
            return op;
        output("%c", get_uint8());
    }
}

static void timeout_expect_op(uint8_t expect) {
    uint8_t got = get_op();
    if(got == expect)
        return;
    output("expected %d, received %d ('%c')\n", expect, got, got);
    // dump out the rest of the string so we can see what is going on.
    while(can_read_timeout(HANDOFF_FD, 100000))
        output("%c", get_op());
}

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

// Helper methods to implement the filesystem
file_t* make_file(unsigned file_size) {
	file_t* new_file = calloc(1, sizeof(file_t));
	new_file->data = calloc(1, file_size);
	new_file->n_alloc = file_size;
	new_file->n_data = 0;
	return new_file;
}

dirent_t* search_dirent(dirent_t* c, const char* path) {
	assert('/' == path[0]);
	int i = 0;
	while(c[i].name != 0){
		if(0 == strcmp(c[i].name, path)) {
			if(c[i].f == 0) {
				c[i].f = make_file(8192);
			}
			return &c[i];
		}
		i++;
	}
	return 0;
}



static int do_reboot(dirent_t *e, const char *path, const char *buf,
                            size_t size, off_t offset, void *data) {
	//debug("Calling Reboot\n");
	put_uint8(PI_REBOOT | PI_NEED_ACK);
	timeout_expect_op(PI_READY);
	exit(0);
	//try if instead of while to get rid of need for double-tap
	//while(can_read_timeout(HANDOFF_FD, 100)) {
    //    output("%c", get_uint8());
	//}
}

static int do_echo(dirent_t *e, const char *path, const char *buf,
                            size_t size, off_t offset, void *data) {
	//debug("Doing Echo\n");
   	//debug("***********************************************************************************\n");
	
	dirent_t* echo_entry = search_dirent(e, path);
	if(!echo_entry) {
		return -ENOENT;
	}
	
	dirent_t* console_entry = search_dirent(e, "/console");
	if(!console_entry) {
		return -ENOENT;
	}
	
	memcpy(console_entry->f->data + console_entry->f->n_data,
		   echo_entry->f->data + echo_entry->f->n_data, 
		   strlen(echo_entry->f->data + echo_entry->f->n_data));
	
	memset(echo_entry->f->data, 0, echo_entry->f->n_data);
	
	echo_entry->f->n_data = 0;

	memcpy(echo_entry->f->data, buf, strlen(buf));

	console_entry->f->n_data += strlen(console_entry->f->data + console_entry->f->n_data);
	
	
	memcpy((void*)buf, echo_entry->f->data + echo_entry->f->n_data, 
			strlen(echo_entry->f->data + echo_entry->f->n_data));
	
	return 0;
}

static int do_run(dirent_t *e, const char *path, const char *buf,
                            size_t size, off_t offset, void *data) {
	//debug("Running Code\n");

	// Binary has been catted into e->f->data + offset. Dawson's
	// write-up says we can use PI_GET32 and PI_PUT32 to write the binary to the Pi. 
	// Is it ok for us to define equivalents for bytes?

	char code_buf [1024 * 64];
	struct pi_bin_header h;
	
	dirent_t* run_entry = search_dirent(e, path);
	if(!run_entry) {
		return -ENOENT;
	}
	// Equivalent to read_exacting from stdin
	memcpy(&h, run_entry->f->data + run_entry->f->n_data, sizeof h);

	// Equivalent to reading in code
	memcpy((void*)code_buf, 
			run_entry->f->data + run_entry->f->n_data + sizeof h, 
			sizeof code_buf);

	memset(run_entry->f->data, 0, run_entry->f->n_data);
	
	run_entry->f->n_data = 0;
	
	//try if instead of while to get rid of need for double-tap
	put_uint8(PI_PUT_CODE | PI_NEED_ACK);
	//while(can_read_timeout(HANDOFF_FD, 100)) {
    //    output("%c", get_uint8());
	//	}
	timeout_expect_op(PI_READY); 
	put_uint32(h.cookie);
	put_uint32(h.hdrsize);
	put_uint32(h.addr);
	put_uint32(h.nbytes);
	put_uint32(our_crc32_inc(code_buf, h.nbytes, our_crc32(&h, sizeof h)));

	for(int i = 0; i < h.nbytes; i++) {
		put_uint8(code_buf[i]);
	}

	//debug("Finished injecting code\n");

	return 0;
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
    int retv = 0;
    //debug("opening\n");
    
	dirent_t* entry = search_dirent(root, path);
	if(!entry) {
		retv = -ENOENT;
	}

	 //Check Permissions
	//if((fi->flags & 0x3) != O_RDONLY) {
	//	retv = -EACCES;
	//}

	return retv;
}

static int pi_readdir(const char *path, void *buf, fuse_fill_dir_t filler, 
                            off_t offset, struct fuse_file_info *fi) {

	//debug("readdir\n");
    
	if(strcmp(path, "/") != 0)
        return -ENOENT;
    
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	
	for(int i = 0; root[i].name != 0; i++) {
		filler(buf, root[i].name + 1, NULL, 0);
	}
	return 0;
}

static int pi_getattr(const char *path, struct stat *stbuf) {
    //debug("getattr\n");

	memset(stbuf, 0, sizeof(struct stat));
    //debug("After memset\n");
	
	if(strcmp(path, "/") == 0) {
    	//debug("matched with root\n");
		stbuf->st_mode = S_IFDIR | 0777;
		stbuf->st_nlink = 2; //sizeof(root)/sizeof(dirent_t);
	} else {

    	//debug("not root: searching for file\n");
		dirent_t* entry = search_dirent(root, path);
		if(!entry) {
			return -ENOENT;
		}
		stbuf->st_mode = S_IFREG | entry->flags;
		stbuf->st_nlink = 1;

		//st_size CANNOT be 0, or FUSE will not let us read from it
		stbuf->st_size = entry->f->n_data;
		//stbuf->st_size = entry->f->n_alloc;
	}

	return 0;
}

static int pi_read(const char *path, char *buf, size_t size, 
                    off_t offset, struct fuse_file_info *fi) {

    //debug("read\n");
   	//debug("**************************************************************************\n");
	dirent_t* entry = search_dirent(root, path);
	if(!entry) {
		return -ENOENT;
	}

	int data_length = entry->f->n_data;
	//debug("Offset: %lld\n", offset);
	//debug("Length: %zu\n", entry->f->n_data);

	// Need to check offset otherwise FUSE
	// will keep reading consecutively
	if(offset >= data_length) {
		return 0;
	}

	//debug("Contents of file: %s\n", entry->f->data);

	memcpy(buf, entry->f->data, entry->f->n_data);
	
	return entry->f->n_data;
}

static int pi_write(const char *path, const char *buf, size_t size, 
        off_t offset, struct fuse_file_info *fi) {

    //debug("write\n");
   	//debug("**************************************************************************\n");
	
	dirent_t* entry = search_dirent(root, path);
	if(!entry) {
		return -ENOENT;
	}
	
	int data_length = strlen(buf);
    
	if(strcmp(entry->name, "/run.cmd") == 0) {
		//debug("In binary run case\n");
		data_length = size;
	}
	
	//debug("Offset: %lld\n", offset);
	//debug("Length: %zu\n", data_length);

	if(strcmp(entry->name, "/console") != 0) {
		memcpy(entry->f->data + entry->f->n_data, buf, data_length);
	}
	
	(*(entry->on_wr))(&(root[0]), path, buf, size, offset, 0);

	entry->f->n_data += data_length;

	if(strcmp(entry->name, "/run.cmd") == 0) {
		entry->f->n_data = 0;
	}
	/*
	dirent_t* console_entry = search_dirent(root, "/console");
	if(!entry) {
		return -ENOENT;
	}
	*/
	////debug("Memcpy to console\n");
	//memcpy(root[3].f->data + offset, entry->f->data + offset, size);
	////debug("Contents of console: %s", root[3].f->data + offset);

	return size;
}

/** Change the size of a file */
static int pi_truncate(const char *path, off_t length) {
    //debug("truncate\n");
    
	dirent_t* entry = search_dirent(root, path);
	if(!entry) {
		//debug("Entry not found");
		return -ENOENT;
	}
	
	//debug("length param: %lld\n", length);
	//debug("entr length: %zu\n", entry->f->n_data);

	if(length == entry->f->n_data) {
		//debug("Length matched");
		return 0;
	}

	//debug("Reallocing\n");
	void* truncated_buf = realloc(entry->f->data, length);

	////debug("Memsetting\n");
	// memset(truncated_buf + entry->f->n_data, 0, length - entry->f->n_data);

	//debug("Reorganizing\n");
	entry->f->data = truncated_buf;
	
	// THIS MAY BREAK ECHO and READ
	entry->f->n_data = length;
	
	//debug("Returning: %zu\n", entry->f->n_data);
	return entry->f->n_data;
}

static int 
pi_ftruncate(const char *path, off_t offset, struct fuse_file_info *fi) {
    //debug("Calling pi_truncate\n");
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
    -d Enable //debugging output (implies -f).

    -f Run in foreground; this is useful if you're running under a
    //debugger. WARNING: When -f is given, Fuse's working directory is
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
    console = ent_lookup(root, "/console");
#endif

    // can comment this out to devlop without the pi initially.
    if(!fd_is_open(HANDOFF_FD))
        panic("pi-fs: must run with <HANDOFF_FD> as an open file descriptor\n");
    // if you want to toggle if using pi or not should have this as a global variable
    else
        using_pi_p = 1;

    // ping the pi til it echos something back: otherwise we could
    // start shoving stuff into its UART before its booted.
    output("about to check ready\n");
    while(1) {
        put_byte(HANDOFF_FD, PI_READY);
        if(can_read_timeout(HANDOFF_FD, 10000)) {
        	// output("%c", get_uint8());
			timeout_expect_op(PI_READY);
            break;
        }
    }
    output("about to call not main\n");
    return fuse_main(argc, argv, &pi_oper, 0);
}
