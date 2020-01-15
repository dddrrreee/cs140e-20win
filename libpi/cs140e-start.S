#include "rpi-asm.h"

// To keep this in the first portion of the binary.
.section ".text.boot"

.globl _start
_start:
    mov sp, #STACK_ADDR
    mov fp, #0  // I don't think necessary.
    @ bl notmain
    bl _cstart
    bl rpi_reboot // if they return just reboot.
