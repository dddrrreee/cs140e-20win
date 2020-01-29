#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "libunix.h"

// read entire file into buffer.  return it.   zero pads to a
// multiple of 4.
//
// make sure to cleanup!
uint8_t *read_file(unsigned *size, const char *name) {
    uint8_t *buf;

    // read in file via stat
    struct stat stat_buf;
    int status = stat(name, &stat_buf);
    
    // check output of stat
    if (status != 0) {
        abort();
    } else {
        ;
    }
    
    // get total bytes size
    off_t raw_size = stat_buf.st_size;
    
    // pad if not alignment of 4
    off_t remainder = raw_size % 4;
    switch(remainder) {
        case 0: {break;}
        case 1: {raw_size += 3; break;}
        case 2: {raw_size += 2; break;}
        case 3: {raw_size += 1; break;}
        default: {;}
    }
    
    buf = (uint8_t*) calloc(stat_buf.st_size + 4, 1);
    *size = stat_buf.st_size;
    
    // open file
    int fd = open(name, O_RDONLY);
    
    // read file into buffer
    int read_bytes = read(fd, buf, stat_buf.st_size);
    
    close(fd);
    
    if (read_bytes < 0) {
        abort();
    } else {
        ;
    }
    
    return buf;
}
