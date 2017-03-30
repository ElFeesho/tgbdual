//
// Created by Christopher Sawczuk on 15/07/2016.
//

#pragma once

#include <osd_renderer.h>
#include <script_context.h>
#include <input_queue.h>

#include <lua.hpp>
#include <memory>

class macro_runner {
public:
    macro_runner(script_context &scriptContext);

    void loadScript(const std::string &script);

    void activate();

    void tick();

private:
    std::unique_ptr<lua_State,  void(*)(lua_State*)> state;
};

