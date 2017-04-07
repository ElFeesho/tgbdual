//
// Created by Christopher Sawczuk on 15/07/2016.
//

#include <iostream>
#include <map>
#include "lua_macro_runner.h"


static std::map<lua_State *, script_context *> contexts;

lua_macro_runner::lua_macro_runner(script_context &scriptContext) : state{luaL_newstate(), lua_close} {
    luaL_openlibs(state.get());

    contexts[state.get()] = &scriptContext;

    lua_createtable(state.get(), 0, 0);
    lua_setglobal(state.get(), "bridge");

    lua_getglobal(state.get(), "bridge");

    bindFunction("print", [](lua_State *state) -> int {
        script_context *ctx = contexts[state];
        ctx->print_string(lua_tolstring(state, 1, nullptr));
        return 0;
    });

    bindFunction("set_16bit_value", [](lua_State *state) -> int {
        script_context *ctx = contexts[state];
        ctx->set_16bit_value((uint32_t) lua_tointegerx(state, 1, nullptr),
                             (uint16_t) lua_tointegerx(state, 2, nullptr));
        return 0;
    });

    bindFunction("set_8bit_value", [](lua_State *state) -> int {
        script_context *ctx = contexts[state];
        uint32_t address = (uint32_t) lua_tointegerx(state, 1, nullptr);
        uint8_t value = (uint8_t) lua_tointegerx(state, 2, nullptr);

        ctx->set_8bit_value(address, value);
        return 0;
    });

    bindFunction("read_8bit_value", [](lua_State *state) -> int {
        script_context *ctx = contexts[state];
        uint8_t value = ctx->read_8bit_value((uint32_t) lua_tointegerx(state, 1, nullptr));
        lua_pushinteger(state, value);
        return 1;
    });

    bindFunction("read_16bit_value", [](lua_State *state) -> int {
        script_context *ctx = contexts[state];
        uint16_t value = ctx->read_16bit_value((uint32_t) lua_tointegerx(state, 1, nullptr));
        lua_pushinteger(state, value);
        return 1;
    });

    bindFunction("add_rect", [](lua_State *state) -> int {
        script_context *ctx = contexts[state];
        int16_t x = (int16_t) lua_tointegerx(state, 1, nullptr);
        int16_t y = (int16_t) lua_tointegerx(state, 2, nullptr);
        int16_t w = (int16_t) lua_tointegerx(state, 3, nullptr);
        int16_t h = (int16_t) lua_tointegerx(state, 4, nullptr);
        uint32_t s = (uint32_t) lua_tointegerx(state, 5, nullptr);
        uint32_t f = (uint32_t) lua_tointegerx(state, 6, nullptr);
        ctx->add_rect(x, y, w, h, s, f);
        return 0;
    });

    bindFunction("add_image", [](lua_State *state) -> int {
        script_context *ctx = contexts[state];
        const char *name = lua_tolstring(state, 1, nullptr);
        int16_t x = (int16_t) lua_tointegerx(state, 2, nullptr);
        int16_t y = (int16_t) lua_tointegerx(state, 3, nullptr);

        ctx->add_image(name, x, y);
        return 0;
    });

    bindFunction("queue_key", [](lua_State *state) -> int {
        script_context *ctx = contexts[state];
        uint8_t key = (uint8_t) lua_tointegerx(state, 1, nullptr);
        uint32_t when = (uint32_t) lua_tointegerx(state, 2, nullptr);
        uint32_t duration = (uint32_t) lua_tointegerx(state, 3, nullptr);

        ctx->queue_key(key, when, duration);
        return 0;
    });

    lua_createtable(state.get(), 0, 0);
    lua_setglobal(state.get(), "GameBoy");

    lua_getglobal(state.get(), "GameBoy");

    std::map<std::string, int> keymap;
    keymap["KEY_A"] = 0x01;
    keymap["KEY_B"] = 0x02;
    keymap["KEY_SELECT"] = 0x04;
    keymap["KEY_START"] = 0x08;
    keymap["KEY_DOWN"] = 0x10;
    keymap["KEY_UP"] = 0x20;
    keymap["KEY_LEFT"] = 0x40;
    keymap["KEY_RIGHT"] = 0x80;

    for (auto &pair : keymap) {
        lua_pushinteger(state.get(), pair.second);
        lua_setfield(state.get(), -2, pair.first.c_str());
    }
}

void lua_macro_runner::bindFunction(const char *fnName, int (*fn)(lua_State *)) const {
    lua_pushcclosure(state.get(), fn, 0);

    lua_setfield(state.get(), -2, fnName);
}

void lua_macro_runner::activate() {
    invokeLuaFunction("activate");
}

void lua_macro_runner::tick() {
    invokeLuaFunction("tick");
}

void lua_macro_runner::invokeLuaFunction(const char *functionName) const {
    int function = lua_getglobal(state.get(), functionName);
    if (function == LUA_TFUNCTION) {
        if (lua_pcallk(state.get(), 0, 0, 0, 0, 0) != 0) {
            std::stringstream ss;
            ss << "SCRIPTERROR: " << lua_tostring(state.get(), -1);
            contexts[state.get()]->print_string(ss.str());
        }
    }
}

void lua_macro_runner::loadScript(const std::string &script) {
    if (luaL_loadstring(state.get(), script.c_str()) == LUA_OK) {
        contexts[state.get()]->print_string("Loaded script successfully");
        lua_pcallk(state.get(), 0, 0, 0, 0, nullptr);

        invokeLuaFunction("onLoad");
    }
}

bool lua_macro_runner::handleUnhandledCommand(const std::string &command, std::vector<std::string> args) {
    if (lua_getglobal(state.get(), "handleCommand") == LUA_TFUNCTION) {
        lua_pushstring(state.get(), command.c_str());
        lua_createtable(state.get(), args.size(), 0);
        if (args.size() > 0) {
            int i = 0;
            for (std::string &value : args) {
                lua_pushstring(state.get(), value.c_str());
                lua_rawseti(state.get(), -2, ++i);
            }
        }
        if (lua_pcallk(state.get(), 2, 1, 0, 0, 0)) {
            std::stringstream ss;
            ss << "SCRIPTERROR: " << lua_tostring(state.get(), -1);
            contexts[state.get()]->print_string(ss.str());
        }
        return (bool) lua_toboolean(state.get(), lua_gettop(state.get()));
    }
    return false;
}
