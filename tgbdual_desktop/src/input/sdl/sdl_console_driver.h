//
// Created by Christopher Sawczuk on 16/04/2017.
//

#ifndef TGBDUAL_SDL_CONSOLE_DRIVER_H
#define TGBDUAL_SDL_CONSOLE_DRIVER_H


#include <input/console_driver.h>

class sdl_console_driver : public tgb::console_driver {
public:
    void update(key_down down, key_up up, commandkey_down commandkey_down, commandkey_up commandkey_up) override;
};


#endif //TGBDUAL_SDL_CONSOLE_DRIVER_H
