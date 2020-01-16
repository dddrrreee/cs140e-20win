#ifndef __TRACE_H__
#define __TRACE_H__
// engler, cs140e: dirt simple tracing system.

// start tracing : <capture_p>=0 tells the code to immediately emit to the screen.
// otherwise record in a log for later printing by <trace_dump>
// 
//  -error: if you were already tracing.
void trace_start(int capture_p);

// stop tracing.  you can restart again.
//  - error: if you were not already tracing.
void trace_stop(void);

// dump out any saved trace.  <reset_p> means it resets the capture log.
//  - error: if you were not in capture mode.
void trace_dump(int reset_p);

#endif
