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
    uint8_t *buf = 0;

    unimplemented();
    return buf;
}
