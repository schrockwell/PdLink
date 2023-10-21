#include <stdint.h>

#include "constants.h"
#include "goertzel.h"
#include "rx.h"

char isSynced = FALSE;
int rxSample = 0;
int rxBit = 0;
char rxByte = 0;
int16_t rxSamples[SYMBOL_SAMPLE_COUNT];

void rxInit(void)
{
	pd->sound->setMicCallback(&rxMicCallback, NULL, 0);
}

int rxMicCallback(void* context, int16_t* data, int len)
{	
	int i = 0;
	
	if (!isSynced) {
		// Skip ahead in small chunks looking for a bit to decode
		int syncCheckCount = (SYMBOL_SAMPLE_COUNT / 4);
		for (; i < (len - SYMBOL_SAMPLE_COUNT); i += syncCheckCount) {
			int value = goertzel_value(SYMBOL_SAMPLE_COUNT, SAMPLE_RATE, SPACE_FREQ, MARK_FREQ, (data + i));
			
			if (value != -1) {
				// We found a bit!
				isSynced = TRUE;
				rxSample = 0;
				rxBit = 0;
				rxByte = 0;
			}
		}
	}
	
	if (isSynced) {
		for (; i < len; i++) {
			rxSamples[rxSample] = data[i];
			rxSample = (rxSample + 1) % SYMBOL_SAMPLE_COUNT;
			
			if (rxSample == 0) {
				// We filled the sample buffer for one bit
				int value = goertzel_value(SYMBOL_SAMPLE_COUNT, SAMPLE_RATE, SPACE_FREQ, MARK_FREQ, rxSamples);
				
				// TEMP
				// for (int j = 0; j < SYMBOL_SAMPLE_COUNT; j++) {
				// 	pd->system->logToConsole("%d", rxSamples[j]);
				// }
				// return 0;
				
				if (value == -1) {
					// oh nooooooo what to do?
					value = 0;
				}
				
				rxByte = rxByte | (value << rxBit);
				
				rxBit = (rxBit + 1) % 8;
				if (rxBit == 0) {
					// We got a byte, baybeeee
					pd->system->logToConsole("Got: %d", rxByte);
					
					// Reset the accumulator
					rxByte = 0;
					
					// Skip the idle time
					i = i + SYMBOL_SAMPLE_COUNT * START_BITS;
				}
			}
		}
	}
	
	return 1;
}