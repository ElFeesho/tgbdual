#pragma once

#include <input/gamepad_source.h>

class dummy_gamepad_source : public tgb::gamepad_source {
public:
    uint8_t provideState() override;
};
