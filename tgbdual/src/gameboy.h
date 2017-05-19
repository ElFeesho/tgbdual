#pragma once

#include <cstdio>

#include <gb.h>
#include <video_renderer.h>
#include <string>

#include <functional>

#include "link_cable_source.h"

#include <address_scanner.h>
#include <emulation/core_services.h>
#include <rendering/gb_video_renderer.h>
#include <rendering/gb_audio_renderer.h>
#include <input/gb_gamepad_source.h>

class gameboy {
public:
	gameboy(core_services *services, gb_video_renderer::render_callback &&renderCallback, link_cable_source *link_cable_source);

	void addCheat(const std::string &cheat);

	void load_rom(uint8_t *romData, size_t romLength, uint8_t *ram = nullptr, size_t ramLength = 0);

	void save_state(const std::function<uint8_t *(size_t)> &functor);

	void load_state(uint8_t *state);

	void save_sram(const std::function<void(uint8_t *, uint32_t)> &functor);

	void tick();

	void set_speed(uint32_t speed);

	void disableInput();
	void enableInput();

	address_scanner createAddressScanner();

	template<typename T>
	void override_ram(uint32_t address, T value)
	{
		*(T *) (_gb.get_cpu().get_ram() + address) = value;
	}

	template<typename T>
	T read_ram(uint32_t address)
	{
		return *(T *) (_gb.get_cpu().get_ram() + address);
	}

private:
	gb_video_renderer _videoRenderer;
	gb_audio_renderer _audioRenderer;
	gb_gamepad_source _gamepadSource;
	gb _gb;
	address_scanner _address_scanner;
};
