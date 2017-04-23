#pragma once

#include <cstdint>

class gamepad_source {
public:
    virtual ~gamepad_source() = default;

    virtual uint8_t check_pad() = 0;

    virtual void reset_pad() = 0;
};
