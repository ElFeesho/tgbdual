//
// Created by Christopher Sawczuk on 15/07/2016.
//

#include <iostream>
#include <map>
#include "macro_runner.h"


static std::map<lua_State *, script_context *> contexts;

macro_runner::macro_runner(osd_renderer *osd, input_queue *queue, gameboy *gb) : context{osd, queue, gb} {

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

	lua_pushcfunction(state, [](lua_State *state) -> int {
		script_context *ctx = contexts[state];
		ctx->set_16bit_value((uint32_t) lua_tointeger(state, 1), (uint16_t) lua_tointeger(state, 2));
		return 0;
	});
	lua_setfield(state, -2, "set_16bit_value");

	lua_pushcfunction(state, [](lua_State *state) -> int {
		script_context *ctx = contexts[state];
		uint32_t address = (uint32_t) lua_tointeger(state, 1);
		uint8_t value = (uint8_t) lua_tointeger(state, 2);

		ctx->set_8bit_value(address, value);
		return 0;
	});
	lua_setfield(state, -2, "set_8bit_value");


	lua_pushcfunction(state, [](lua_State *state) -> int {
		script_context *ctx = contexts[state];
		uint8_t value = ctx->read_8bit_value((uint32_t) lua_tointeger(state, 1));
		lua_pushinteger(state, value);
		return 1;
	});

	lua_setfield(state, -2, "read_8bit_value");


	lua_pushcfunction(state, [](lua_State *state) -> int {
		script_context *ctx = contexts[state];
		uint16_t value = ctx->read_16bit_value((uint32_t) lua_tointeger(state, 1));
		lua_pushinteger(state, value);
		return 1;
	});

	lua_setfield(state, -2, "read_16bit_value");

	lua_pushcfunction(state, [](lua_State *state) -> int {
		script_context *ctx = contexts[state];
		int x = lua_tointeger(state, 1);
		int y = lua_tointeger(state, 2);
		int w = lua_tointeger(state, 3);
		int h = lua_tointeger(state, 4);
		int s = lua_tointeger(state, 5);
		int f = lua_tointeger(state, 6);
		ctx->add_rect(x, y, w, h, s, f);
		return 0;
	});

	lua_setfield(state, -2, "add_rect");

	lua_pushcfunction(state, [](lua_State *state) -> int {
		script_context *ctx = contexts[state];
		const char *name = lua_tostring(state, 1);
		int x = lua_tointeger(state, 2);
		int y = lua_tointeger(state, 3);

		ctx->add_image(name, x, y);
		return 0;
	});
	lua_setfield(state, -2, "add_image");

	lua_pushcfunction(state, [](lua_State *state) -> int {
		script_context *ctx = contexts[state];
		ctx->clear_canvas();
		return 0;
	});

	lua_setfield(state, -2, "clear_canvas");


	lua_pushcfunction(state, [](lua_State *state) -> int {
		script_context *ctx = contexts[state];
		uint8_t key = lua_tointeger(state, 1);
		uint32_t when = lua_tointeger(state, 2);
		uint32_t duration = lua_tointeger(state, 3);

		ctx->queue_key(key, when, duration);
		return 0;
	});
	lua_setfield(state, -2, "queue_key");
}

macro_runner::~macro_runner() {
	lua_close(state);
}

void macro_runner::activate() {
	int function = lua_getglobal(state, "activate");
	if (function == LUA_TFUNCTION) {
		lua_callk(state, 0, 0, 0, [](lua_State *state, int status, lua_KContext ctx) -> int {
			contexts[state]->print_string("Failed to execute macro");
			return 0;
		});

	}
}

void macro_runner::loadScript(const std::string &script) {
	if (luaL_loadstring(state, script.c_str()) == LUA_OK) {
		context.print_string("Loaded script successfully");
		lua_pcall(state, 0, 0, 0);
	}
}
