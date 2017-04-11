//
// Created by Christopher Sawczuk on 16/07/2016.
//

#pragma once

#include <functional>

#include <gameboy.h>
#include "osd_renderer.h"
#include "input_queue.h"

class script_context {
public:
    using script_command = std::function<void(std::vector<std::string>)>;

    script_context(osd_renderer *osd, input_queue *queue, gameboy *gb, std::function<void(const std::string&, script_command)> consoleCommandRegistrar, std::function<void(const std::string&)> consoleCommandUnregistrar);

    void print_string(const std::string &);

    void set_16bit_value(uint32_t address, uint16_t value);

    void set_8bit_value(uint32_t address, uint8_t value);

    uint8_t read_8bit_value(uint32_t address);

    uint16_t read_16bit_value(uint32_t address);

    void add_image(const std::string &name, int16_t x, int16_t y);

    void add_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t stroke, uint32_t fill);

    void add_text(const std::string &message, int16_t x, int16_t y);

    void queue_key(uint8_t key, uint32_t when, uint32_t duration);

    void register_command(const std::string &cmdName, script_command command);
    void unregister_command(const std::string &cmdName);

private:
    osd_renderer *_osd;
    input_queue *_queue;
    gameboy *_gameboy;
    std::function<void(const std::string&, script_command)> _consoleCommandRegistrar;
    std::function<void(const std::string&)> _consoleCommandUnregistrar;
};

