#include <complex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

#include "constants.h"
#include "main.h"
#include "goertzel.h"
#include "tx.h"

PlaydateAPI* pd;

#ifdef _WINDLL
__declspec(dllexport)
#endif
int
eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg)
{
	if ( event == kEventInitLua )
	{
		pd = playdate;
		
		txInit();
		
		const char* err;

		if ( !pd->lua->addFunction(say_hello, "pd_link.say_hello", &err) )
			pd->system->logToConsole("%s:%i: addFunction failed, %s", __FILE__, __LINE__, err);
			
		if ( !pd->lua->addFunction(do_goertzel, "pd_link.do_goertzel", &err) )
			pd->system->logToConsole("%s:%i: addFunction failed, %s", __FILE__, __LINE__, err);
			
		if ( !pd->lua->addFunction(tx_start, "pd_link.tx_start", &err) )
			pd->system->logToConsole("%s:%i: addFunction failed, %s", __FILE__, __LINE__, err);
			
		if ( !pd->lua->addFunction(tx_end, "pd_link.tx_end", &err) )
			pd->system->logToConsole("%s:%i: addFunction failed, %s", __FILE__, __LINE__, err);
			
		if ( !pd->lua->addFunction(tx_enqueue, "pd_link.tx_enqueue", &err) )
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
	// This just runs the Goertzel detection for a simulated sine wave at the mark frequency
	float data[NUM_SAMPLES];
	float frequency = MARK_FREQ;
	float samplePeriod = 1.0f / SAMPLE_RATE;
	float angularFrequency = 2.0f * (float)M_PI * frequency;
	for (int i = 0; i < NUM_SAMPLES; ++i) {
		float time = i * samplePeriod; // the time at which the sample is taken
		data[i] = sinf(angularFrequency * time); // the sample itself
	}
	
	uint8_t result = goertzel_value(NUM_SAMPLES, SAMPLE_RATE, SPACE_FREQ, MARK_FREQ, data);

	pd->lua->pushInt(result);
	return 1;
}


static int tx_init(lua_State* L) {
	txInit();
	return 0;
}

static int tx_start(lua_State* L) {
	txStart();
	return 0;
}

static int tx_end(lua_State* L) {
	txEnd();
	return 0;
}

static int tx_enqueue(lua_State* L) {
	char byte = (char)(pd->lua->getArgInt(1));
	
	txEnqueue(byte);
	
	return 0;
};


