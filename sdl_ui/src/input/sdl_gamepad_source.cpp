#include <chrono>
#include <algorithm>
#include "sdl_gamepad_source.h"

sdl_gamepad_source::sdl_gamepad_source() {
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

void sdl_gamepad_source::update_pad_state(const SDL_Event &e) {
	if (input_sources.find(e.type) != input_sources.end()) {
		padState = input_sources[e.type]->provide_input(padState, e);
	}
}

uint8_t sdl_gamepad_source::check_pad() {

	uint32_t epoch = SDL_GetTicks();
	for (auto event : pending_events) {

		if (event.when > epoch) {
			break;
		}

		if (epoch > event.when && epoch < event.when + event.duration) {
			padState |= event.key;
		}
		else {
			padState &= ~(event.key);
		}
	}

	if (pending_events.size() > 0) {
		pending_events.erase(
				std::remove_if(pending_events.begin(), pending_events.end(), [](const input_event &event) -> bool {
					return event.when + event.duration < SDL_GetTicks();
				}), pending_events.end());
	}

	return padState;
}

void sdl_gamepad_source::queue_key(const input_event &event) {
	pending_events.emplace_back(input_event{event.key, event.when + SDL_GetTicks(), event.duration});
}


