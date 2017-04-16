//
// Created by Christopher Sawczuk on 14/04/2017.
//

#include "emulator_time.h"

uint32_t emulator_time::current_time() {
    return emulator_time::_time_provider();
}

void emulator_time::set_time_provider(emulator_time::time_provider provider) {
    emulator_time::_time_provider = provider;
}

void emulator_time::set_sleep_provider(emulator_time::sleep_provider provider) {
    _sleep_provider = provider;
}

void emulator_time::sleep(uint32_t duration) {
    _sleep_provider(duration);
}

emulator_time::time_provider emulator_time::_time_provider = []{ return 0; };
emulator_time::sleep_provider emulator_time::_sleep_provider = [](long){};
