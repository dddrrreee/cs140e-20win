#include "rpi-interrupts.h"
void data_abort_vector(unsigned pc) { INT_UNHANDLED("data abort", pc); }
