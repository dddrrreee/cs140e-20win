// fake implementations of pi-code so that we can check hall in user space.
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "pi-test.h"

// in case they didn't include rpi
void (nop)(void) {}
