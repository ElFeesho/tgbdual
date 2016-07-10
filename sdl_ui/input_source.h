#pragma once

#include <SDL.h>
#include <cstdint>

class input_source
{
public:
	virtual uint8_t provide_input(uint8_t initial_state, const SDL_Event &ev) = 0;
};

