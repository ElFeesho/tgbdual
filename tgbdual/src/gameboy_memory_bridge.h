#pragma once

#include <memory_bridge.h>
#include <gameboy.h>

class gameboy_memory_bridge : public memory_bridge {
public:
    explicit gameboy_memory_bridge(gameboy &gb);

    uint8_t read_8bit(uint32_t addr) override;

    void write_8bit(uint32_t addr, uint8_t val) override;

    uint16_t read_16bit(uint32_t addr) override;

    void write_16bit(uint32_t addr, uint16_t val) override;
private:
    gameboy &_gb;
};