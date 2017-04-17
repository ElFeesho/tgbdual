#pragma once

#include <SDL.h>
#include <gamepad_source.h>
#include <map>
#include <vector>

#include <input/gamepad_source.h>
#include <input/sys_command_source.h>
#include "input_queue.h"


class sdl_gamepad_source : public tgb::gamepad_source, public tgb::sys_command_source {
public:
    uint8_t provideState() override;

private:
    void provideSysCommand(std::function<void()> saveState, std::function<void()> loadState, std::function<void()> toggleSpeed, std::function<void()> quit, std::function<void()> activateScript,
                           std::function<void()> openConsole) override;

private:
    std::map<SDLKey, uint8_t> _keyMap{{SDLK_z,      0x01},
                                      {SDLK_x,      0x02},
                                      {SDLK_RSHIFT, 0x04},
                                      {SDLK_RETURN, 0x08},
                                      {SDLK_DOWN,   0x10},
                                      {SDLK_UP,     0x20},
                                      {SDLK_LEFT,   0x40},
                                      {SDLK_RIGHT,  0x80}};
};

