#pragma once

#include <string>

#include "io/file_buffer.h"

class RomFile {
public:
    RomFile(const std::string &romPath);

    file_buffer &rom();

    file_buffer &sram();

    file_buffer &state();

    void writeSram(uint8_t *sramData, uint32_t length);

    void writeState(uint8_t *stateData, uint32_t length);

private:
    std::string _romPath;
    std::string _sramPath;
    std::string _statePath;

    file_buffer _romBuffer;
    file_buffer _sramBuffer;
    file_buffer _stateBuffer;
};
