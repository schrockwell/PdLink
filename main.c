#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

static PlaydateAPI* pd = NULL;
static int say_hello(lua_State* L);

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
	}
	
	return 0;
}

static int say_hello(lua_State* L)
{
	pd->lua->pushString("Hello, world!");
	return 1;
}
