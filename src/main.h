#ifndef _main_h_
#define _main_h_

static int say_hello(lua_State* L);
static int do_goertzel(lua_State* L);

static int tx_init(lua_State* L);
static int tx_start(lua_State* L);
static int tx_end(lua_State* L);
static int tx_enqueue(lua_State* L);

#endif