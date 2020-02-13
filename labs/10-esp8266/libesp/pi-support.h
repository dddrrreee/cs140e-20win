#ifndef __PI_SUPPORT_H__
#define __PI__SUPPORT_H__
// these are the pi-specific includes we need.

#include <ctype.h>
#include "rpi.h"

#define snprintf snprintk
#define malloc kmalloc

// client has to define the following.
typedef void *esp_handle_t;
#endif
