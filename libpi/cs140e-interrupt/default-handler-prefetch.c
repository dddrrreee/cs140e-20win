#include "rpi-interrupts.h"
void prefetch_abort_vector(unsigned pc) { INT_UNHANDLED("prefetch abort", pc); }
