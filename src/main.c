#include <complex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"
#include "goertzel.h"

#define NUM_SAMPLES  64
#define SPACE_FREQ   5000.0f
#define MARK_FREQ    10000.0f
#define SAMPLE_RATE  44100.0f

static PlaydateAPI* pd = NULL;
static int say_hello(lua_State* L);
static int do_goertzel(lua_State* L);

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
			
		if ( !pd->lua->addFunction(do_goertzel, "pd_link.do_goertzel", &err) )
			pd->system->logToConsole("%s:%i: addFunction failed, %s", __FILE__, __LINE__, err);
	}
	
	return 0;
}

static int say_hello(lua_State* L)
{
	pd->lua->pushString("Hello, world!");
	return 1;
}

static int do_goertzel(lua_State* L) {
	// Generate samples for an example sine wave at MARK_FREQ
	float frequency = MARK_FREQ;
	float samplePeriod = 1.0f / SAMPLE_RATE;
	float angularFrequency = 2.0f * (float)M_PI * frequency;
	float data[NUM_SAMPLES];
	for (int i = 0; i < NUM_SAMPLES; ++i) {
		float time = i * samplePeriod; // the time at which the sample is taken
		data[i] = sinf(angularFrequency * time); // the sample itself
	}
	
	uint8_t result = goertzel_value(NUM_SAMPLES, SAMPLE_RATE, SPACE_FREQ, MARK_FREQ, data);

	pd->lua->pushInt(result);
	return 1;
}



