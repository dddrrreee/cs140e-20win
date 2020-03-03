#include "rpi-interrupts.h"
void reset_vector(unsigned pc) { INT_UNHANDLED("reset", pc); }
