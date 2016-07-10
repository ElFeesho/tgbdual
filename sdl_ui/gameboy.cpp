#include <stdexcept>

#include "gameboy.h"

gameboy::gameboy(renderer *render, link_cable_source *link_cable_source) : 
	_gb{render,[this]{}, [&]{return link_cable_source->readByte();}, [&](uint8_t data) { link_cable_source->sendByte(data); }}, 
	_renderer{render}
{
}

void gameboy::load_rom(uint8_t *romData, uint32_t romLength, uint8_t *ram, uint32_t ramLength)
{
    _gb.load_rom(romData, romLength, ram, ramLength);
}

void gameboy::save_state(std::function<uint8_t*(uint32_t)> functor) {
    _gb.save_state_mem(functor(_gb.get_state_size()));
}

void gameboy::load_state(uint8_t *state) {
    _gb.restore_state_mem(state);
}

void gameboy::save_sram(std::function<void(uint8_t*,uint32_t)> functor) {
    functor(_gb.get_rom()->get_sram(), _gb.get_rom()->get_sram_size());
}

void gameboy::tick()
{
	_gb.run();
}

void gameboy::fastForward() {
	_gb.set_skip(9);
}

void gameboy::normalForward() {
	_gb.set_skip(0);
}

void gameboy::provideInput(std::function<uint8_t(uint8_t)> provideInputFunctor)
{
	_renderer->set_pad(provideInputFunctor(_renderer->check_pad()));
}