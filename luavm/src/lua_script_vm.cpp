#include <iostream>
#include <map>
#include <lua_script_vm.h>

void storeContext(script_services *scriptContext, lua_State *luaVm) {
    lua_pushlightuserdata(luaVm, scriptContext);
    lua_setglobal(luaVm, "__context");
}

script_services *getContext(lua_State *luaVm) {
    lua_getglobal(luaVm, "__context");
    auto context = (script_services*)lua_touserdata(luaVm, lua_gettop(luaVm));
    lua_pop(luaVm, 1);
    return context;
}

lua_script_vm::lua_script_vm(script_services *scriptContext) : state{luaL_newstate(), lua_close} {
    luaL_openlibs(state.get());

    storeContext(scriptContext, state.get());

    lua_createtable(state.get(), 0, 0);
    lua_setglobal(state.get(), "GameBoy");

    lua_getglobal(state.get(), "GameBoy");

    bindFunction("print", [](lua_State *state) -> int {
        getContext(state)->print_string(lua_tolstring(state, 1, nullptr));
        return 0;
    });

    bindFunction("set_16bit_value", [](lua_State *state) -> int {
        getContext(state)->set_16bit_value((uint32_t) lua_tointegerx(state, 1, nullptr),
                                           (uint16_t) lua_tointegerx(state, 2, nullptr));
        return 0;
    });

    bindFunction("set_8bit_value", [](lua_State *state) -> int {
        auto address = (uint32_t) lua_tointegerx(state, 1, nullptr);
        auto value = (uint8_t) lua_tointegerx(state, 2, nullptr);

        getContext(state)->set_8bit_value(address, value);
        return 0;
    });

    bindFunction("get_8bit_value", [](lua_State *state) -> int {
        uint8_t value = getContext(state)->read_8bit_value((uint32_t) lua_tointegerx(state, 1, nullptr));
        lua_pushinteger(state, value);
        return 1;
    });

    bindFunction("get_16bit_value", [](lua_State *state) -> int {
        uint16_t value = getContext(state)->read_16bit_value((uint32_t) lua_tointegerx(state, 1, nullptr));
        lua_pushinteger(state, value);
        return 1;
    });

    bindFunction("add_rect", [](lua_State *state) -> int {
        auto x = (int16_t) lua_tointegerx(state, 1, nullptr);
        auto y = (int16_t) lua_tointegerx(state, 2, nullptr);
        auto w = (int16_t) lua_tointegerx(state, 3, nullptr);
        auto h = (int16_t) lua_tointegerx(state, 4, nullptr);
        auto s = (uint32_t) lua_tointegerx(state, 5, nullptr);
        auto f = (uint32_t) lua_tointegerx(state, 6, nullptr);
        getContext(state)->add_rect(x, y, w, h, s, f);
        return 0;
    });

    bindFunction("add_text", [](lua_State *state) -> int {
        const char *text = lua_tolstring(state, 1, nullptr);
        auto x = (int16_t) lua_tointegerx(state, 2, nullptr);
        auto y = (int16_t) lua_tointegerx(state, 3, nullptr);

        getContext(state)->add_text(text, x, y);
        return 0;
    });

    bindFunction("add_image", [](lua_State *state) -> int {
        const char *name = lua_tolstring(state, 1, nullptr);
        auto x = (int16_t) lua_tointegerx(state, 2, nullptr);
        auto y = (int16_t) lua_tointegerx(state, 3, nullptr);

        getContext(state)->add_image(name, x, y);
        return 0;
    });

    bindFunction("queue_key", [](lua_State *state) -> int {
        auto key = (uint8_t) lua_tointegerx(state, 1, nullptr);
        auto when = (uint32_t) lua_tointegerx(state, 2, nullptr);
        auto duration = (uint32_t) lua_tointegerx(state, 3, nullptr);

        getContext(state)->queue_key(key, when, duration);
        return 0;
    });

    bindFunction("register_console_command", [](lua_State *state) -> int {
        const char *commandName = lua_tostring(state, 1);
        if (lua_isfunction(state, 2)) {
            int funcRef = luaL_ref(state, LUA_REGISTRYINDEX);
            getContext(state)->register_command(commandName, [=](std::vector<std::string> args) {
                lua_rawgeti(state, LUA_REGISTRYINDEX, funcRef);
                lua_createtable(state, (int) args.size(), 0);
                if (!args.empty()) {
                    int i = 0;
                    for (std::string &value : args) {
                        lua_pushstring(state, value.c_str());
                        lua_rawseti(state, -2, ++i);
                    }
                }
                lua_pcallk(state, 1, 0, 0, 0, nullptr);
            });
        }

        return 0;
    });

    registerConstants();
}

void lua_script_vm::registerConstants() const {
    lua_getglobal(state.get(), "GameBoy");

    std::map<std::string, int> keymap{
            {"KEY_A",      0x01},
            {"KEY_B",      0x02},
            {"KEY_SELECT", 0x04},
            {"KEY_START",  0x08},
            {"KEY_DOWN",   0x10},
            {"KEY_UP",     0x20},
            {"KEY_LEFT",   0x40},
            {"KEY_RIGHT",  0x80},
    };

    for (auto &pair : keymap) {
        lua_pushinteger(state.get(), pair.second);
        lua_setfield(state.get(), -2, pair.first.c_str());
    }
}

void lua_script_vm::bindFunction(const char *fnName, int (*fn)(lua_State *)) const {
    lua_pushcclosure(state.get(), fn, 0);

    lua_setfield(state.get(), -2, fnName);
}

void lua_script_vm::activate() {
    invokeLuaFunction("activate");
}

void lua_script_vm::tick() {
    invokeLuaFunction("tick");
}

void lua_script_vm::invokeLuaFunction(const char *functionName) const {
    lua_getglobal(state.get(), functionName);
    int function = lua_type(state.get(), -1);
    if (function == LUA_TFUNCTION) {
        if (lua_pcallk(state.get(), 0, 0, 0, 0, nullptr) != 0) {
            getContext(state.get())->print_string("Lua Error: " + std::string(lua_tolstring(state.get(), -1, nullptr)));
        }
    }
}

void lua_script_vm::loadScript(const std::string &script) {
    if (luaL_loadstring(state.get(), script.c_str()) == LUA_OK) {
        getContext(state.get())->print_string("Loaded script successfully");
        lua_pcallk(state.get(), 0, 0, 0, 0, nullptr);
        invokeLuaFunction("onLoad");
    }
}

bool lua_script_vm::handleUnhandledCommand(const std::string &command, std::vector<std::string> args) {
    lua_getglobal(state.get(), "handleCommand");
    int function = lua_type(state.get(), -1);
    if (function == LUA_TFUNCTION) {
        lua_pushstring(state.get(), command.c_str());
        lua_createtable(state.get(), (int) args.size(), 0);
        if (!args.empty()) {
            int i = 0;
            for (std::string &value : args) {
                lua_pushstring(state.get(), value.c_str());
                lua_rawseti(state.get(), -2, ++i);
            }
        }
        if (lua_pcallk(state.get(), 2, 1, 0, 0, nullptr) != 0) {
            getContext(state.get())->print_string("Lua Error: " + std::string(lua_tolstring(state.get(), -1, nullptr)));
        }
        return (bool) lua_toboolean(state.get(), lua_gettop(state.get()));
    }
    return false;
}


