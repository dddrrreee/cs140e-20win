#ifndef __TIMER_INTERRUPT_H__
#define __TIMER_INTERRUPT_H__

#include "rpi.h"
#include "rpi-armtimer.h"
#include "rpi-interrupts.h"

void timer_interrupt_init(unsigned ncycles);
#endif
