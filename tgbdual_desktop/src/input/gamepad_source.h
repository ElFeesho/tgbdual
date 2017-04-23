#pragma once

#include <cstdint>

namespace tgb {
    class gamepad_source {
    public:
        virtual ~gamepad_source() = default;

        virtual uint8_t provideState() = 0;
    };
};
