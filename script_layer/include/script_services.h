#pragma once

#include <string>
#include <vector>
#include <functional>

class script_services {
public:
    using script_command = std::function<void(std::vector<std::string>)>;

    virtual ~script_services() {}

    virtual void print_string(const std::string &) = 0;

    virtual void set_16bit_value(uint32_t address, uint16_t value) = 0;

    virtual void set_8bit_value(uint32_t address, uint8_t value) = 0;

    virtual uint8_t read_8bit_value(uint32_t address) = 0;

    virtual uint16_t read_16bit_value(uint32_t address) = 0;

    virtual void add_image(const std::string &name, int16_t x, int16_t y) = 0;

    virtual void add_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t stroke, uint32_t fill) = 0;

    virtual void add_text(const std::string &message, int16_t x, int16_t y) = 0;

    virtual void queue_key(uint8_t key, uint32_t when, uint32_t duration) = 0;

    virtual void register_command(const std::string &cmdName, script_command command) = 0;
};