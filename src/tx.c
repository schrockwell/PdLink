#include "tx.h"
#include "constants.h"

// Playdate audio
SoundChannel *txChannel = NULL;
SoundSource *txSource = NULL;

// Circular buffer of data to send
#define TX_BUFFER_SIZE 1024
char txBuffer[TX_BUFFER_SIZE];
int txBufferAt = 0;
int txBufferEnd = 0;
int txBufferBit = 0;
int txBitSample = 0;

// Precomputed lookup tables for the TX sine waves
int16_t txSignalMark[SYMBOL_SAMPLE_COUNT];
int16_t txSignalSpace[SYMBOL_SAMPLE_COUNT];

int txInit(void)
{
	txChannel = pd->sound->channel->newChannel();
	pd->sound->channel->setPan(txChannel, -1.0f); // left
	
	// Pre-populate mark and space signals for fast lookup
	float angularFrequencyMark = 2.0f * (float)M_PI * MARK_FREQ;
	float angularFrequencySpace = 2.0f * (float)M_PI * SPACE_FREQ;
	for (int i = 0; i < SYMBOL_SAMPLE_COUNT; i++) {
		// Use a cosine shape as the envelope: _/--\_
		// This is supposed to divide by 2.0, but for some reason that's not enough attenuation and results in clipping
		// so let's go with a fudge factor and divide by 3.0 instead to attenuate it a bit more.
		float envelope = (1 - cosf(2 * M_PI * i / (float)SYMBOL_SAMPLE_COUNT)) / 3.0f;
		// pd->system->logToConsole("%d %f", i, envelope);
		
		float time = i * SAMPLE_PERIOD;
		txSignalMark[i] = (int)(envelope * sinf(angularFrequencyMark * time) * (INT16_MAX - 1)); 
		txSignalSpace[i] = (int)(envelope * sinf(angularFrequencySpace * time) * (INT16_MAX - 1));
	}
	
	return 0;
}

int txStart(void)
{
	if (txSource != NULL) {
		return -1;
	}
	
	txSource = pd->sound->channel->addCallbackSource(txChannel, &txCallback, NULL, 0);
	
	return 0;
}

int txEnd(void)
{
	if (txSource == NULL) {
		return -1;
	}
	
	pd->system->realloc(txSource, 0);
	txSource = NULL;
	
	return 0;
}

// https://sdk.play.date/2.0.3/Inside%20Playdate%20with%20C.html#f-sound.channel.addCallbackSource
int txCallback(void* context, int16_t* left, int16_t* right, int len)
{
	// Return early if there's nothing to send
	if (txBufferAt == txBufferEnd) {
		return 0;
	}
	
	// Fill out the samples
	for (int i = 0; i < len; i++) {
		// Increment the next sample
		txBitSample = (txBitSample + 1) % SYMBOL_SAMPLE_COUNT;
		
		if (txBitSample == 0) {
			// Roll over to the next bit
			txBufferBit = (txBufferBit + 1) % (START_BITS + 8);
			
			if (txBufferBit == 0) {
				// Roll over to the next byte
				txBufferAt = (txBufferAt + 1) % TX_BUFFER_SIZE;
			}
		}
		
		// If there are no more bytes to send, emit silence for the rest of the data
		if (txBufferAt == txBufferEnd) {
			left[i] = 0;
			continue;
		}
		
		if (txBufferBit < START_BITS) {
			// Emit silence for start bits
			left[i] = 0;
		} else {
			// Determine the TX bit
			char byte = txBuffer[txBufferAt];
			char isMark = (byte & (1 << (txBufferBit - START_BITS))) > 0;
			
			// Copy from the pre-calculated signal waveform
			int16_t *signal = isMark ? txSignalMark : txSignalSpace;
			left[i] = signal[txBitSample];
		}
	}
	
	return 1;
}

int txEnqueue(char byte)
{
	txBuffer[txBufferEnd] = byte;
	txBufferEnd = (txBufferEnd + 1) % TX_BUFFER_SIZE;
	
	if (txBufferEnd == txBufferAt) {
		// uh-oh, the circular buffer ran out
	}

	return 1;
}