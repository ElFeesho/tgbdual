#pragma once

#include <stdio.h>

#include <gb.h>
#include <renderer.h>
#include <string>

#include <functional>

#include "link_cable_source.h"

#include <address_scanner.h>

class gameboy {
public:
	gameboy(renderer *render, gamepad_source *gp_source, link_cable_source *link_cable_source);

	void load_rom(uint8_t *romData, uint32_t romLength, uint8_t *ram, uint32_t ramLength);

	void save_state(std::function<uint8_t *(size_t)> functor);

	void load_state(uint8_t *state);

	void save_sram(std::function<void(uint8_t *, uint32_t)> functor);

	void tick();

	void set_speed(uint32_t speed);

	template<typename T>
	address_scan_result scan_for_address(T value) {
		return _address_scanner.find_value(value);
	}

	template<typename T>
	void override_ram(uint32_t address, T value)
	{
		*(T*)(_gb.get_cpu()->get_ram()+address) = value;
	}

	template<typename T>
	T read_ram(uint32_t address)
	{
		return *(T*)(_gb.get_cpu()->get_ram()+address);
	}

private:
	gb _gb;
	address_scanner _address_scanner;
};