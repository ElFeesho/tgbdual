//
// Created by Christopher Sawczuk on 15/07/2016.
//

#pragma once

#include "osd_renderer.h"
#include "script_context.h"
#include "input_queue.h"
#include <lua.hpp>

class macro_runner {
public:
	macro_runner(osd_renderer *osd, input_queue *queue, gameboy *gb);
	~macro_runner();

	void loadScript(const std::string &script);
	void activate();

private:
	lua_State *state;

	script_context context;
};

