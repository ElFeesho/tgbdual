#include "keyboard_input_source.h"

keyboard_input_source::keyboard_input_source() {
    keyValues[SDLK_DOWN]    = 0x10;
    keyValues[SDLK_UP]      = 0x20;
    keyValues[SDLK_LEFT]    = 0x40;
    keyValues[SDLK_RIGHT]   = 0x80;

    keyValues[SDLK_z]       = 0x01;
    keyValues[SDLK_x]       = 0x02;
    keyValues[SDLK_RSHIFT]  = 0x04;
    keyValues[SDLK_RETURN]  = 0x08;
}

uint8_t keyboard_input_source::provide_input(uint8_t initial_state, const SDL_Event &ev)
{
    auto sym = ev.key.keysym.sym;
    if (keyValues.find(sym) != keyValues.end())
    {
        if (ev.type == SDL_KEYDOWN)
        {
            initial_state |= keyValues[sym];
        }
        else
        {
            initial_state &= ~keyValues[sym];
        }
    }
    return initial_state;
}
