#pragma once

#include <script_vm.h>
#include <osd_renderer.h>
#include <script_context.h>
#include <input_queue.h>

#include <lua.hpp>
#include <memory>

class lua_script_vm : public script_vm {
public:
    explicit lua_script_vm(script_services *scriptServices);

    void loadScript(const std::string &script) override;

    void activate() override;

    void tick() override;

    bool handleUnhandledCommand(const std::string &command, std::vector<std::string> args) override;
private:
    std::unique_ptr<lua_State,  void(*)(lua_State*)> state;

    void invokeLuaFunction(const char *functionName) const;

    void bindFunction(const char *fnName, int (*fn)(lua_State *)) const;

    void registerConstants() const;
};

