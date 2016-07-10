#pragma once

#include <cstdint>

class gamepad_source {
public:
    virtual ~gamepad_source() {}
    virtual uint8_t check_pad() = 0;
};
