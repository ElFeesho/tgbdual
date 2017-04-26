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

void rom_file::writeSram(uint8_t *sramData, size_t length) {
    std::fstream sram{_sramPath, std::ios::out};
    sram.write((const char *) sramData, length);
    sram.close();

    _sramBuffer = file_buffer{_sramPath};
}

void rom_file::writeState(uint8_t *stateData, size_t length) {
    std::fstream state{_statePath, std::ios::out};
    state.write((const char *) stateData, length);
    state.close();

    _stateBuffer = file_buffer{_statePath};
}

uint8_t *rom_file::loadRom() {
    return _romBuffer;
}

size_t rom_file::romLength() {
    return _romBuffer.length();
}

void rom_file::saveState(uint8_t *state, size_t length) {
    writeState(state, length);
}

uint8_t *rom_file::loadState() {
    return _stateBuffer;
}

void rom_file::saveSram(uint8_t *sram, size_t length) {
    writeSram(sram, length);
}

uint8_t *rom_file::loadSram() {
    return _sramBuffer;
}

size_t rom_file::sramLength() {
    return _sramBuffer.length();
}
