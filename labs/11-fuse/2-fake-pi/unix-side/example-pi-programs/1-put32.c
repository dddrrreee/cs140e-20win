/* do a single put32 to a harmless space. */
#include "rpi.h"

void notmain(void) {
    PUT32(0x1000, 0xfeedface);
}
