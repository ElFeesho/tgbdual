#pragma once

#include <cstdint>
#include <cstddef>

namespace tgb {
    class rom {
    public:
        virtual ~rom() = default;

        virtual uint8_t *loadRom() = 0;
        virtual size_t romLength() = 0;
        virtual void saveState(uint8_t *state, size_t length) = 0;
        virtual uint8_t *loadState() = 0;
        virtual void saveSram(uint8_t *sram, size_t length) = 0;
        virtual uint8_t *loadSram() = 0;

        virtual size_t sramLength() = 0;
    };
}