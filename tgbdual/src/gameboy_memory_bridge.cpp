#include "gameboy_memory_bridge.h"

gameboy_memory_bridge::gameboy_memory_bridge(gameboy &gb) : _gb{gb} {

}

uint8_t gameboy_memory_bridge::read_8bit(uint32_t addr) {
    return _gb.read_ram<uint8_t>(addr);
}

void gameboy_memory_bridge::write_8bit(uint32_t addr, uint8_t val) {
    _gb.override_ram(addr, val);
}

uint16_t gameboy_memory_bridge::read_16bit(uint32_t addr) {
    return (((uint16_t) _gb.read_ram<uint8_t>(addr)) << 8) | (_gb.read_ram<uint8_t>(addr + 1));
}

void gameboy_memory_bridge::write_16bit(uint32_t addr, uint16_t val) {
    _gb.override_ram(addr - 1, val & 0x00ff);
    _gb.override_ram(addr, (val & 0xff00) >> 8);
}
