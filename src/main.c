#include <complex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"
#include "fft.h"

static PlaydateAPI* pd = NULL;
static int say_hello(lua_State* L);
static int do_fft(lua_State* L);

#ifdef _WINDLL
__declspec(dllexport)
#endif
int
eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg)
{
	if ( event == kEventInitLua )
	{
		pd = playdate;
		
		const char* err;

		if ( !pd->lua->addFunction(say_hello, "pd_link.say_hello", &err) )
			pd->system->logToConsole("%s:%i: addFunction failed, %s", __FILE__, __LINE__, err);
			
		if ( !pd->lua->addFunction(do_fft, "pd_link.do_fft", &err) )
			pd->system->logToConsole("%s:%i: addFunction failed, %s", __FILE__, __LINE__, err);
	}
	
	return 0;
}

static int say_hello(lua_State* L)
{
	pd->lua->pushString("Hello, world!");
	return 1;
}

static int do_fft(lua_State* L) {
	float x[N]; // Input array (time domain)
	float complex X[N / 2 + 1]; // Output array (frequency domain) - non-redundant part

	// Fill 'x' array with some data, e.g., a sine wave, here a simple example is used
	for (int i = 0; i < N; i++) {
		// x[i] = sinf(2 * PI * i / N * 3); // Example of data input
		x[i] = sinf(2 * PI * i / N * 3) + sinf(2 * PI * i / N * 5); // Example of data input
	}

	// Calculate FFT
	fft_real(x, X, N);

	// Store the peaks as a bitmask (N = 64 results in 32 useful bins)
	uint16_t result = 0;
	
	for (int i = 0; i < N / 2 + 1; i++) {
		float mag = cabsf(X[i]);
		
		// Set threshold to 50% magnitude
		if (mag > ((float)N / 4)) {
			result = result | (1 << i);
		}
		// pd->system->logToConsole("X[%d] = %lf", i, mag);
		// pd->system->logToConsole("X[%d] = %lf + %lfj", i, creal(X[i]), cimag(X[i]));
	}

	pd->lua->pushInt(result);
	return 1;
}