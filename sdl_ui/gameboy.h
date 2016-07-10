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

	void load_rom(const std::string &romFilename);

	void save_state();
	void load_state();
	void save_sram();

	void tick();

	void fastForward();
	void normalForward();

	void provideInput(std::function<uint8_t(uint8_t)> provideInputFunctor);

private:

	gb _gb;
	renderer *_renderer{nullptr};

	std::string _romFile;
	std::string _saveFile;
	std::string _stateFile;

};

