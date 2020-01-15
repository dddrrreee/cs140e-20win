#include "rpi-internal.h"

// should probably change this (or support code) so that if the uart isn't enabled,
// we don't try to print and maybe flash an LED.
void (panic)(const char *file, int lineno, const char *msg) {
    putk("<PANIC>:");
    printk("%s:%d: PANIC PANIC PANIC: %s\n", file, lineno, msg);
    rpi_reboot();
}
