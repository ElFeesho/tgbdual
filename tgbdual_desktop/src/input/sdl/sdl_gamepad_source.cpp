#include <chrono>
#include "sdl_gamepad_source.h"

uint8_t sdl_gamepad_source::provideState() {
    uint8_t state = 0;

    uint8_t *keys = SDL_GetKeyState(nullptr);

    for (auto &pair : _keyMap) {
        state |= keys[pair.first] * pair.second;
    }

    return state;
}


