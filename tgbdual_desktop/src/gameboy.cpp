#include <stdexcept>
#include <emulation/core_services.h>

#include "gameboy.h"

gameboy::gameboy(core_services *services, gb_video_renderer::render_callback renderCallback,  link_cable_source *link_cable_source)
    : _videoRenderer{services->videoRenderer(), renderCallback, 100}, _audioRenderer{services->audioRenderer()}, _gamepadSource{services->gamepadSource()}, _gb{&_videoRenderer, &_audioRenderer, &_gamepadSource, [this] {}, [link_cable_source] { return link_cable_source->readByte(); }, [link_cable_source](uint8_t data) { link_cable_source->sendByte(data); }}, _address_scanner{_gb.create_address_scanner()} {
}

void gameboy::load_rom(uint8_t *romData, uint32_t romLength, uint8_t *ram, uint32_t ramLength) {
    _gb.load_rom(romData, romLength, ram, ramLength);
}

void gameboy::save_state(const std::function<uint8_t *(size_t)> &functor) {
    _gb.save_state_mem(functor(_gb.get_state_size()));
}

void gameboy::load_state(uint8_t *state) {
    _gb.restore_state_mem(state);
}

void gameboy::save_sram(const std::function<void(uint8_t *, uint32_t)> &functor) {
    functor(_gb.get_rom()->get_sram(), _gb.get_rom()->get_sram_size());
}

void gameboy::tick() {
    _gb.run();
}

void gameboy::set_speed(uint32_t speed) {
    _gb.set_skip(speed);
}

address_scanner gameboy::createAddressScanner() {
    return _gb.create_address_scanner();
}

void gameboy::disableInput() {
    _gamepadSource.reset_pad();
    _gamepadSource.disable();
}

void gameboy::enableInput() {
    _gamepadSource.enable();
}

