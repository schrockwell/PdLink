#ifndef _constants_h_
#define _constants_h_

extern PlaydateAPI* pd;

#define NUM_SAMPLES   64
#define SPACE_FREQ    5000.0f
#define MARK_FREQ     10000.0f
#define SAMPLE_RATE   44100.0f
#define SAMPLE_PERIOD 0.00002268f

// 44,100 / 40 = 1102.5 baud
#define SYMBOL_SAMPLE_COUNT 40

#endif