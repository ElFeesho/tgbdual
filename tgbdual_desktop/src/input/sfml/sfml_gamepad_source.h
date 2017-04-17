//
// Created by Christopher Sawczuk on 17/04/2017.
//

#ifndef TGBDUAL_SFML_GAMEPAD_SOURCE_H
#define TGBDUAL_SFML_GAMEPAD_SOURCE_H


#include <input/gamepad_source.h>
#include <SFML/Graphics/RenderWindow.hpp>

class sfml_gamepad_source : public tgb::gamepad_source {
public:
    uint8_t provideState() override;
};


#endif //TGBDUAL_SFML_GAMEPAD_SOURCE_H
