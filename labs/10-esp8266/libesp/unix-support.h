#ifndef __UNIX_SUPPORT_H__
#define __UNIX_SUPPORT_H__
// these are the unix-specific includes we need.

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include "libunix.h"


// client has to define the following.
typedef int esp_handle_t;

void esp_log(int fd);

#endif
