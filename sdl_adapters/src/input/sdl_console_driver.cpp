//
// Created by Christopher Sawczuk on 16/04/2017.
//

#include <SDL/SDL_events.h>
#include <map>
#include "sdl_console_driver.h"

void sdl_console_driver::update(tgb::console_driver::key_down down, tgb::console_driver::key_up up,
                                tgb::console_driver::commandkey_down commandkey_down, tgb::console_driver::commandkey_up commandkey_up) {
    std::map<SDLKey, tgb::console_driver::CommandKey> commandKeys = {
            {SDLK_UP, tgb::console_driver::CommandKey::UP},
            {SDLK_DOWN, tgb::console_driver::CommandKey::DOWN},
            {SDLK_LEFT, tgb::console_driver::CommandKey::LEFT},
            {SDLK_RIGHT, tgb::console_driver::CommandKey::RIGHT},
            {SDLK_RETURN, tgb::console_driver::CommandKey::RETURN},
            {SDLK_TAB, tgb::console_driver::CommandKey::TAB},
            {SDLK_BACKQUOTE, tgb::console_driver::CommandKey::CLOSE_CONSOLE},
            {SDLK_BACKSPACE, tgb::console_driver::CommandKey::BACKSPACE}
    };

    SDL_Event ev;
    while(SDL_PollEvent(&ev)) {
        if (ev.type == SDL_KEYDOWN)
        {
            SDLKey key = ev.key.keysym.sym;
            if (commandKeys.find(key) != commandKeys.end())
            {
                commandkey_down(commandKeys[key]);
            }
            else
            {
                if (key != SDLK_UNKNOWN && key != SDLK_LSHIFT) {
                    if (ev.key.keysym.mod == KMOD_LSHIFT) {
                        if (key >= SDLK_a && key <= SDLK_z) {
                            down(key - 32);
                        }
                        else {
                            if (key == SDLK_MINUS)
                            {
                                down('_');
                            }
                        }
                    } else {
                        down(key);
                    }
                }
            }
        } else if (ev.type == SDL_KEYUP) {
            SDLKey key = ev.key.keysym.sym;
            if (commandKeys.find(key) != commandKeys.end())
            {
                commandkey_up(commandKeys[key]);
            }
            else
            {
                if (key != SDLK_UNKNOWN) {
                    if (ev.key.keysym.mod == KMOD_LSHIFT) {
                        up(key - 32);
                    } else {
                        up(key);
                    }
                }
            }
        }
    }
}
