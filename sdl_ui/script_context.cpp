//
// Created by Christopher Sawczuk on 16/07/2016.
//

#include <gameboy.h>
#include "script_context.h"

static inline uint16_t swap_endianness(uint16_t value)
{
	return (uint16_t) (((value & 0xff) << 8) | ((value & 0xff00) >> 16));
}

script_context::script_context(osd_renderer *osd, gameboy *gb) : _osd{osd}, _gameboy{gb} {

}

void script_context::print_string(const std::string &msg) {
	_osd->display_message(msg, 1500);
}

void script_context::set_16bit_value(uint32_t address, uint16_t value) {
	_gameboy->override_ram(address-1, value & 0x00ff);
	_gameboy->override_ram(address, (value & 0xff00) >> 8);
}

void script_context::set_8bit_value(uint32_t address, uint8_t value) {
	_gameboy->override_ram(address, value);
}

uint8_t script_context::read_8bit_value(uint32_t address)
{
	return _gameboy->read_ram<uint8_t>(address);
}

uint16_t script_context::read_16bit_value(uint32_t address) {
	return (((uint16_t)_gameboy->read_ram<uint8_t>(address))<<8) | (_gameboy->read_ram<uint8_t>(address-1));
}
