#pragma once

#include <cstdint>

class memory_bridge {
public:
    virtual ~memory_bridge(){}
    virtual uint8_t read_8bit(uint32_t addr) = 0;
    virtual void write_8bit(uint32_t addr, uint8_t val) = 0;
    virtual uint16_t read_16bit(uint32_t addr) = 0;
    virtual void write_16bit(uint32_t addr, uint16_t val) = 0;
};