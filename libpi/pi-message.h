#ifndef __PI_MESSAGE_H__
#define __PI_MESSAGE_H__

// opcodes we use to communicate with pi.  
//  - these should fit in a single byte.  
//  - if you bitwise-OR in PI_NEED_ACK the pi will reply 
//    with the same opcode when it does the operation.
//  - most opcodes will have an associated structure that
//    aggregates all the information their messages contains
//
// for lab 11 we only use a few of these, i've included them
// so that you get a feel of things that might make sense to do.
//
// maybe cut some of these out.
// 
//
enum {
    PI_NULL = 0,        // no opcode.
    PI_READY,           // pi sends this at the start.
    
    PI_PRINT_STRING,    // { nbytes:uint32, string:nbytes long}
    PI_PRINT_CHAR,      // { uint8 }
    PI_REBOOT,          // cause the PI to reboot.

    // following four are modeled after bootloader.
    PI_PUT_CODE_INFO,
    PI_GET_CODE_INFO,   // not used today: lets the pi request code
    PI_PUT_CODE,
    PI_GET_CODE,        // not used today: let's the pi request code.

    // the next four mirror the corresponding pi routines.  
    // remote code can easily easily control the pi GPIO pins.

    PI_GPIO_READ,           // expects a uint8 pin, returns uint8
    PI_GPIO_READ_REPLY,     // returns the uint8 val read from the pin

    PI_GPIO_WRITE,          // expects a uint8 pin and uint8 val

    PI_GPIO_SET_INPUT,      // expects a uint8 pin
    PI_GPIO_SET_OUTPUT,     // expects a uint8 pin

    // using only these two you can cause the pi to do almost 
    // anyting by sending and receiving these.   
    //
    // if you use interrupts to receive these messages you can use
    // them to build a debugger (these need some way to "peek" and 
    // "poke at different memory addresses).
    PI_PUT32,
    PI_GET32,
    PI_GET32_REPLY,

    /******************************************************
     * only add opcodes after this point *unless* you can 
     * guarantee that you can recompile all code that uses
     * the old values.  otherwise it's going to break all
     * over the place --- this gives you a feel for why 
     * its so hard to change networking protocols and generally
     * any low level format :)
     */

    PI_END,         // do not put opcodes after this.

    PI_NEED_ACK = 1<<7    // bit-wise or this with any previous opcode to 
                    // force a reply
};

static void check(void) {
    // make sure we can use the upper bit
    AssertNow(PI_END < 127);
}

struct pi_bin_header {
    uint32_t cookie, hdrsize, addr, nbytes;
};

#endif 

