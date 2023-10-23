#include <stdint.h>
#include <stdlib.h>

#include "constants.h"
#include "goertzel.h"
#include "rx.h"

char isSynced = FALSE;
int rxSample = 0;
int rxBit = 0;
char rxByte = 0;
int16_t rxSamples[SYMBOL_SAMPLE_COUNT];

int rollover = 0;
int tempBitCounter = 0;
int tempByteCounter = 0;

void rxInit(void)
{
	pd->sound->setMicCallback(&rxMicCallback, NULL, 0);
}

int rxMicCallback(void* context, int16_t* data, int len)
{	
	int i = rollover;
	rollover = 0;
	
	for (; i < len; i++) {
		if (!isSynced) {
			// Sliding window
			for (int j = 0; j < SYMBOL_SAMPLE_COUNT - 1; j++) {
				rxSamples[j] = rxSamples[j + 1];
			}
			rxSamples[SYMBOL_SAMPLE_COUNT - 1] = data[i];
			
			int value = goertzel_value(SYMBOL_SAMPLE_COUNT, SAMPLE_RATE, SPACE_FREQ, MARK_FREQ, rxSamples);
			
			if (value == -1) {
				continue;
			} else {
				// We found a bit! So add a fudge factor
				// pd->system->logToConsole("Synced!");
				// i = i + SYMBOL_SAMPLE_COUNT / 2;
				
				isSynced = TRUE;
				rxSample = 0;
				rxBit = 0;
				rxByte = 0;
				
				// If we extended past the end of this RX buffer, then keep the rollover sample count
				// and bail out early
				if (i >= len) {
					rollover = len - i;
					return 1;
				} else {
					rollover = 0;
				}
			}
		}
		
		// Now we are synced, so start decoding bits
		if (isSynced) {
			// TODO: This is not technically correct for the FIRST bit after syncing
			rxSamples[rxSample] = data[i];
			rxSample = (rxSample + 1) % SYMBOL_SAMPLE_COUNT;
		
			if (rxSample == 0) {
				// We filled the sample buffer for one bit
				int value = goertzel_value(SYMBOL_SAMPLE_COUNT, SAMPLE_RATE, SPACE_FREQ, MARK_FREQ, rxSamples);
				if (value == -1) {
					// oh no :(
					value = 0;
				}
				
				rxByte = rxByte | (value << rxBit);
				
				rxBit = (rxBit + 1) % 8;
				if (rxBit == 0) {
					// We got a byte, baybeeee
					pd->system->logToConsole("Got: %d", (uint8_t)rxByte);
					
					// Reset the accumulator
					rxByte = 0;
					
					isSynced = FALSE;
					for (int j = 0; j < SYMBOL_SAMPLE_COUNT; j++) {
						rxSamples[j] = 0;
					}
	
					tempByteCounter++;
				}
				
				tempBitCounter++;
			}
		}
	}
	
	return 1;
}