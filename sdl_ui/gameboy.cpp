#include <stdexcept>

#include <SDL.h>

#include "gameboy.h"

static uint8_t *file_read(const std::string &name, int *size) {
    uint8_t *dat = 0;
    FILE *file = fopen(name.c_str(), "rb");
    if (!file)
    {
        throw std::domain_error("Failed to load "+name+" rom file");
    }
    
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);
    dat = new uint8_t[*size];
    
    fread(dat, 1, *size, file);
    
    fclose(file);
    return dat;
}

gameboy::gameboy(renderer *render, link_cable_source *link_cable_source) : 
	_gb{render,[this]{save_sram();}, [&]{return link_cable_source->readByte();}, [&](uint8_t data) { link_cable_source->sendByte(data); }}, 
	_renderer{render}
{
}

void gameboy::load_rom(const std::string &romFilename)
{
	_romFile = romFilename;
	_saveFile = romFilename.substr(0, romFilename.find_last_of(".")) + ".sav";
	_stateFile = romFilename.substr(0, romFilename.find_last_of(".")) + ".sv0";
	
	int size = 0;
    uint8_t *dat = file_read(_romFile, &size);
    
    int ram_size;
    uint8_t *ram = file_read(_saveFile, &ram_size);
    
    _gb.load_rom(dat, size, ram, ram_size);

    delete[] dat;
    delete[] ram;
}

void gameboy::save_state() {
	FILE *file = fopen(_stateFile.c_str(), "wb");
    _gb.save_state(file);
    fclose(file);
}

void gameboy::load_state() {
	FILE *file = fopen(_stateFile.c_str(), "rb");
    _gb.restore_state(file);
    fclose(file);
}

void gameboy::save_sram() {
	FILE *fsu = fopen(_saveFile.c_str(), "w");
    fwrite((void*)_gb.get_rom()->get_sram(), _gb.get_rom()->get_sram_size(), 1, fsu);
    fclose(fsu);
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