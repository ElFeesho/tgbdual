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
	void add_image(const std::string &name, int16_t x, int16_t y);
	void add_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t stroke, uint32_t fill);
	void clear_canvas();

private:
	osd_renderer *_osd;
	gameboy *_gameboy;

};

