//
// Created by Christopher Sawczuk on 16/07/2016.
//

#pragma once

#include <functional>
#include <vector>
#include "osd_renderer.h"
#include "memory_bridge.h"
#include "input_queue.h"
#include "script_services.h"

class script_context : public script_services {
public:
    using script_command = std::function<void(std::vector<std::string>)>;

    script_context(osd_renderer *osd, input_queue *queue, memory_bridge *memoryBridge, std::function<void(const std::string &, script_command)> consoleCommandRegistrar);

    void print_string(const std::string &message) override;

    void set_16bit_value(uint32_t address, uint16_t value) override;

    void set_8bit_value(uint32_t address, uint8_t value) override;

    uint8_t read_8bit_value(uint32_t address) override;

    uint16_t read_16bit_value(uint32_t address) override;

    void add_image(const std::string &name, int16_t x, int16_t y) override;

    void add_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t stroke, uint32_t fill) override;

    void add_text(const std::string &message, int16_t x, int16_t y) override;

    void queue_key(uint8_t key, uint32_t when, uint32_t duration) override;

    void register_command(const std::string &cmdName, script_command command) override;

private:
    osd_renderer *_osd;
    input_queue *_queue;
    memory_bridge *_memoryBridge;
    std::function<void(const std::string&, script_command)> _consoleCommandRegistrar;
};

