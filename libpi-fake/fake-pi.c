// fake implementations of pi-code so that we can check hall in user space.
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "pi-test.h"

void dev_barrier(void) {
    trace("dev barrier\n");
}

void uart_init(void) { 
    trace("uart\n");
}

// in case they didn't include rpi
void (nop)(void) {}
