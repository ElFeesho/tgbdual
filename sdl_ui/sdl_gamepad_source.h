#pragma once

#include <SDL.h>
#include <gamepad_source.h>
#include <map>

#include "keyboard_input_source.h"
#include "joystick_input_source.h"


class sdl_gamepad_source : public gamepad_source
{
public:
	sdl_gamepad_source();
	void update_pad_state(const SDL_Event &ev);
	uint8_t check_pad();

private:
	uint8_t padState { 0 };
	std::map<int, input_source*> input_sources;
	
    keyboard_input_source keyboardSource;
    joystick_input_source joystickSource;
};

