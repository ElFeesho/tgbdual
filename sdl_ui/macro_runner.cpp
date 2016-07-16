//
// Created by Christopher Sawczuk on 15/07/2016.
//

#include <iostream>
#include <map>
#include "macro_runner.h"


static std::map<lua_State*, script_context*> contexts;

macro_runner::macro_runner(osd_renderer *osd) : _osd{osd}, context{osd} {

	state = luaL_newstate();
	luaL_openlibs(state);

	contexts[state] = &context;

	lua_newtable(state);
	lua_setglobal(state, "bridge");

	lua_getglobal(state, "bridge");
	lua_pushcfunction(state, [](lua_State *state) -> int {
		script_context *ctx = contexts[state];
		ctx->print_string(lua_tostring(state, 1));
		return 0;
	});
	lua_setfield(state, -2, "print");
}

macro_runner::~macro_runner() {
	lua_close(state);
}

void macro_runner::activate() {
	int function = lua_getglobal(state, "activate");
	if (function == LUA_TFUNCTION)
	{
		lua_callk(state, 0, 0, 0, [](lua_State *L, int status, lua_KContext ctx) -> int {
			contexts[state]->print_string("Failed to execute macro");
			return 0;
		});

	}
}

void macro_runner::loadScript(uint8_t *script) {
	if (luaL_loadstring(state, (const char*)script) == LUA_OK)
	{
		_osd->display_message("Loaded script successfully", 2000);
		lua_pcall(state, 0, 0, 0);
	}
}
