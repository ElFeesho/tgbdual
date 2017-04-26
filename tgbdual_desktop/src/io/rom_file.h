#pragma once

#include <string>
#include <rom.h>

#include "io/file_buffer.h"

class rom_file : public tgb::rom {
public:
    explicit rom_file(const std::string &romPath);

    uint8_t *loadRom() override;

    size_t romLength() override;

    void saveState(uint8_t *state, size_t length) override;

    uint8_t *loadState() override;

    void saveSram(uint8_t *sram, size_t length) override;

    uint8_t *loadSram() override;

    uint32_t sramLength() override;

    file_buffer &rom();

    file_buffer &sram();

    file_buffer &state();

    void writeSram(uint8_t *sramData, size_t length);

    void writeState(uint8_t *stateData, size_t length);

private:
    std::string _romPath;
    std::string _sramPath;
    std::string _statePath;

    file_buffer _romBuffer;
    file_buffer _sramBuffer;
    file_buffer _stateBuffer;
};
