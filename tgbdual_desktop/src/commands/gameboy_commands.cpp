#include "gameboy_commands.h"

#include <io/memory_buffer.h>

void registerGameBoyCommands(console &console, gameboy &gbInst, rom_file &romFile) {
    console.addCommand("save", [&](std::vector<std::string> args) {
        memory_buffer buffer;

        gbInst.save_state([&](uint32_t length) -> uint8_t* {
            buffer.alloc(length);
            return (uint8_t *) buffer;
        });

        romFile.writeState(buffer, buffer.length());
        console.addOutput("State saved");
    });

    console.addCommand("load", [&](std::vector<std::string> args) {
        gbInst.load_state(romFile.state());
        console.addOutput("State loaded");
    });
}