#include "rpi-interrupts.h"
void undefined_instruction_vector(unsigned pc) { INT_UNHANDLED("undefined inst", pc); }
