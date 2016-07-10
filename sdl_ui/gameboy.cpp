#include <stdexcept>

#include <SDL.h>

#include <fstream>

#include "gameboy.h"

static uint8_t *file_read(const std::string &name, int *size) {
    uint8_t *dat = 0;

    std::ifstream file{name, std::ios::binary | std::ios::in};
    if (!file.good())
    {
        throw std::domain_error("Failed to load "+name);
    }
    
    file.seekg(0, file.end);
    *size = file.tellg();
    file.seekg(0, file.beg);
    dat = new uint8_t[*size];
    file.read((char*)dat, *size);
    file.close();
    return dat;
}

gameboy::gameboy(renderer *render, link_cable_source *link_cable_source) : 
	_gb{render,[this]{}, [&]{return link_cable_source->readByte();}, [&](uint8_t data) { link_cable_source->sendByte(data); }}, 
	_renderer{render}
{
}

void gameboy::load_rom(const std::string &romFilename)
{
	_romFile = romFilename;
	
	int size = 0;
    uint8_t *dat = file_read(_romFile, &size);
    
    int ram_size;
    uint8_t *ram = file_read(romFilename.substr(0, romFilename.find_last_of(".")) + ".sav", &ram_size);
    
    _gb.load_rom(dat, size, ram, ram_size);

    delete[] dat;
    delete[] ram;
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