#pragma once

#include <cstdint>

class link_cable_source {
public:
    virtual ~link_cable_source() = default;

    virtual uint8_t readByte() = 0;

    virtual void sendByte(uint8_t) = 0;
};