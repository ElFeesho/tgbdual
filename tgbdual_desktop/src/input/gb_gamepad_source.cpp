//
// Created by Christopher Sawczuk on 16/04/2017.
//

#include <emulator_time.h>
#include "gb_gamepad_source.h"

gb_gamepad_source::gb_gamepad_source(tgb::gamepad_source *gamepad_source) : _gamepad_source{gamepad_source} {

}

uint8_t gb_gamepad_source::check_pad() {
    _padState = _gamepad_source->provideState();

    uint32_t epoch = emulator_time::current_time();
    for (auto &event : pending_events) {

        if (event.when > epoch) {
            continue;
        }

        if (epoch > event.when && epoch < event.end) {
            _padState |= event.key;
        }
    }

    if (pending_events.size() > 0) {
        pending_events.erase(
                std::remove_if(pending_events.begin(), pending_events.end(), [&](const input_event &event) -> bool {
                    if (event.end <= epoch) {
                        _padState &= ~(event.key);
                        return true;
                    }
                    return false;
                }), pending_events.end());
    }

    return _padState;
}

void gb_gamepad_source::reset_pad() {
    _padState = 0;
}

void gb_gamepad_source::queue_key(const input_event &event) {
    pending_events.emplace_back(input_event{event.key, (uint32_t) (event.when + emulator_time::current_time()), event.duration});
}
