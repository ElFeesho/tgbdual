#include "sdl_gamepad_source.h"

sdl_gamepad_source::sdl_gamepad_source()
{
	input_sources[SDL_KEYDOWN] = &keyboardSource;
    input_sources[SDL_KEYUP] = &keyboardSource;

	SDL_InitSubSystem(SDL_INIT_JOYSTICK);

    if (SDL_NumJoysticks() > 0) {
        auto joy = SDL_JoystickOpen(0);

        if (joy) {
            input_sources[SDL_JOYBUTTONDOWN] = &joystickSource;
            input_sources[SDL_JOYBUTTONUP] = &joystickSource;
            input_sources[SDL_JOYAXISMOTION] = &joystickSource;
        } else {
            printf("Couldn't open Joystick 0\n");
        }
    }
}

void sdl_gamepad_source::update_pad_state(const SDL_Event &e)
{
	if (input_sources.find(e.type) != input_sources.end())
    {
        padState = input_sources[e.type]->provide_input(padState, e);
    }
}

uint8_t sdl_gamepad_source::check_pad() {
	return padState;
}


