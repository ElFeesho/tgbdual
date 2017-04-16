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

void sdl_gamepad_source::provideSysCommand(std::function<void()> saveState, std::function<void()> loadState, std::function<void()> toggleSpeed, std::function<void()> quit,
                                           std::function<void()> activateScript, std::function<void()> openConsole) {
    SDL_Event ev;
    while(SDL_PollEvent(&ev)) {
        if (ev.type == SDL_KEYDOWN)
        {
            SDLKey key = ev.key.keysym.sym;
            if (key == SDLK_F5) {
                saveState();
            }
            else if(key == SDLK_F7)
            {
                loadState();
            }
            else if(key == SDLK_TAB)
            {
                toggleSpeed();
            }
            else if(key == SDLK_ESCAPE)
            {
                quit();
            }
            else if(key == SDLK_SPACE)
            {
                activateScript();
            }
            else if(key == SDLK_BACKQUOTE)
            {
                openConsole();
            }
        }
    }
}


