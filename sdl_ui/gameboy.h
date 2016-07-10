#pragma once

#include <stdio.h>

#include <string>
#include <gb.h>
#include <renderer.h>

#include <functional>

#include "link_cable_source.h"

class gameboy
{
public:
	gameboy(renderer *render, link_cable_source *link_cable_source);

	void load_rom(uint8_t *romData, uint32_t romLength, uint8_t *ram, uint32_t ramLength);

	void save_state(std::function<uint8_t*(uint32_t)> functor);
	void load_state(uint8_t *state);
	void save_sram(std::function<void(uint8_t*,uint32_t)> functor);

	void tick();

	void fastForward();
	void normalForward();

	void provideInput(std::function<uint8_t(uint8_t)> provideInputFunctor);

private:

	gb _gb;
	renderer *_renderer{nullptr};

	std::string _romFile;
};

