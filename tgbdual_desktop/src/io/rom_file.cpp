//
// Created by Christopher Sawczuk on 06/04/2017.
//
#include <fstream>
#include <iomanip>

#include "rom_file.h"

#include <sys/stat.h>

rom_file::rom_file(const std::string &romPath) :
        _romPath{romPath},
        _sramPath{romPath.substr(0, romPath.find_last_of(".")) + ".sav"},
        _statePath{romPath.substr(0, romPath.find_last_of(".")) + ".sv0"},
        _romBuffer{_romPath},
        _sramBuffer{_sramPath},
        _stateBuffer{_statePath} {
    struct stat st;
    if (stat(_romPath.c_str(), &st) != 0) {
        throw std::domain_error("Failed to open rom: " + _romPath);
    }
}

file_buffer &rom_file::rom() {
    return _romBuffer;
}

file_buffer &rom_file::sram() {
    return _sramBuffer;
}

file_buffer &rom_file::state() {
    return _stateBuffer;
}

void rom_file::writeSram(uint8_t *sramData, uint32_t length) {
    std::fstream sram{_sramPath, std::ios::out};
    sram.write((const char *) sramData, length);
    sram.close();

    _sramBuffer = file_buffer{_sramPath};
}

void rom_file::writeState(uint8_t *stateData, uint32_t length) {
    std::fstream state{_statePath, std::ios::out};
    state.write((const char *) stateData, length);
    state.close();

    _stateBuffer = file_buffer{_statePath};
}