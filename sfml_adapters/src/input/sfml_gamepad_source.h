#pragma once

#include <input/gamepad_source.h>
#include <SFML/Graphics/RenderWindow.hpp>

class sfml_gamepad_source : public tgb::gamepad_source {
public:
    uint8_t provideState() override;
};
