#pragma once

#include <SDL.h>
#include <gamepad_source.h>
#include <map>
#include <vector>

#include "keyboard_input_source.h"
#include "joystick_input_source.h"
#include "input_queue.h"


class sdl_gamepad_source : public gamepad_source, public input_queue
{
public:
	sdl_gamepad_source();
	void update_pad_state(const SDL_Event &ev);
	virtual uint8_t check_pad() override;

	virtual void queue_key(const input_event &event) override;

private:
	uint8_t padState { 0 };
	std::map<int, input_source*> input_sources;
	
    keyboard_input_source keyboardSource;
    joystick_input_source joystickSource;

	std::vector<input_event> pending_events;
};

