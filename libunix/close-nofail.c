#include <unistd.h>
#include "libunix.h"

#define close_nofail(fd) no_fail(close(fd))
