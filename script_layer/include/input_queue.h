#pragma once

#include <cstdint>

struct input_event {
    input_event(uint8_t _key, uint32_t _when, uint32_t _duration) : key{_key}, when{_when}, duration{_duration}, end{_when + _duration} {}

    uint8_t key;
    uint32_t when;
    uint32_t duration;
    uint32_t end;
};

class input_queue {
public:
    virtual ~input_queue() = default;

    virtual void queue_key(const input_event &) = 0;
};