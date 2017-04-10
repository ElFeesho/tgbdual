//
// Created by Christopher Sawczuk on 16/07/2016.
//

#include <osd_renderer.h>
#include <script_context.h>

script_context::script_context(osd_renderer *osd, input_queue *queue, gameboy *gb) :
        _osd{osd},
        _queue{queue},
        _gameboy{gb} {

}

void script_context::print_string(const std::string &msg) {
    _osd->display_message(msg, 1500);
}

void script_context::set_16bit_value(uint32_t address, uint16_t value) {
    _gameboy->override_ram(address - 1, value & 0x00ff);
    _gameboy->override_ram(address, (value & 0xff00) >> 8);
}

void script_context::set_8bit_value(uint32_t address, uint8_t value) {
    _gameboy->override_ram(address, value);
}

uint8_t script_context::read_8bit_value(uint32_t address) {
    return _gameboy->read_ram<uint8_t>(address);
}

uint16_t script_context::read_16bit_value(uint32_t address) {
    return (((uint16_t) _gameboy->read_ram<uint8_t>(address)) << 8) | (_gameboy->read_ram<uint8_t>(address + 1));
}

void script_context::add_image(const std::string &name, int16_t x, int16_t y) {
    _osd->add_image({name, x, y});
}

void script_context::add_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t stroke, uint32_t fill) {
    _osd->add_rect({x, y, w, h, stroke, fill});
}

void script_context::queue_key(uint8_t key, uint32_t when, uint32_t duration) {
    _queue->queue_key({key, when, duration});
}

void script_context::add_text(const std::string &message, int16_t x, int16_t y) {
    _osd->add_text(message, x, y);
}

