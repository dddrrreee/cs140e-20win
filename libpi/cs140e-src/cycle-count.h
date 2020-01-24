#ifndef __CYCLE_COUNT_H__
// use r/pi cycle counters so that we can do tighter timings.

// must do init first.
#define cycle_cnt_init() do {                                   \
    unsigned in = 1;                                            \
    asm volatile("MCR p15, 0, %0, c15, c12, 0" :: "r"(in));     \
} while(0)

// read.  should add a write().
#define cycle_cnt_read() ({	 					\
	unsigned _out;							\
  	asm volatile ("MRC p15, 0, %0, c15, c12, 1" : "=r"(_out));	\
	_out;								\
})

#endif
