#pragma once

#include <gamepad_source.h>
#include <input_queue.h>
#include <vector>
#include "gamepad_source.h"

class gb_gamepad_source : public gamepad_source, public input_queue {
public:
    gb_gamepad_source(tgb::gamepad_source *gamepad_source);

    void enable();
    void disable();

    uint8_t check_pad() override;

    void reset_pad() override;

    void queue_key(const input_event &event) override;

private:
    tgb::gamepad_source *_gamepad_source;
    std::vector<input_event> pending_events;
    uint8_t _padState;

    bool _enabled{true};
};