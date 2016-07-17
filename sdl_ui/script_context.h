//
// Created by Christopher Sawczuk on 16/07/2016.
//

#pragma once

#include "osd_renderer.h"
#include "gameboy.h"

class script_context {
public:
	script_context(osd_renderer *osd, gameboy *gb);

	void print_string(const std::string&);
	void set_16bit_value(uint32_t address, uint16_t value);
	void set_8bit_value(uint32_t address, uint8_t value);
	uint8_t read_8bit_value(uint32_t address);
	uint16_t read_16bit_value(uint32_t address);

private:
	osd_renderer *_osd;
	gameboy *_gameboy;
};

