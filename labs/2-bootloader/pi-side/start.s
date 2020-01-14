@ based on dwelch's start for bootloader.

// To keep this in the first portion of the binary.
.section ".text.boot"


.globl _start
_start:
    b skip
.space 0x200000-0x8004,0
skip:
    mov sp,#0x08000000
    bl notmain
hang: b reboot

@ cannot overwrite this code or anything after it --- make sure
@ sure you check  that the code sent to the bootloader
@ does not collide with the address of <BRANCHTO>
.globl BRANCHTO
BRANCHTO:
    bx r0
