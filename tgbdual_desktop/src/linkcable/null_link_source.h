#pragma once

#include "link_cable_source.h"

class null_link_source : public link_cable_source {
public:
    uint8_t readByte() override;
    void sendByte(uint8_t data) override;
};

