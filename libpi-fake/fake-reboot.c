#include "pi-test.h"

void rpi_reboot(void) {
    trace("raw reboot\n");
    exit(0);
}
void clean_reboot(void) {
    trace("clean reboot\n");
    exit(0);
}

