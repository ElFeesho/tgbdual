#pragma once

#include <functional>

class emulator_time {
public:
    using time_provider = std::function<uint32_t()>;
    using sleep_provider = std::function<void(uint32_t)>;
    static uint32_t current_time();
    static void set_time_provider(time_provider provider);
    static void set_sleep_provider(sleep_provider provider);
    static void sleep(uint32_t duration);
    static time_provider _time_provider;
    static sleep_provider _sleep_provider;

};
