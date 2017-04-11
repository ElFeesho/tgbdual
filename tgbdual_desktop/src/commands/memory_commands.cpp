#include "memory_commands.h"

#include <vector>
#include <string>
#include <sstream>

void registerMemoryCommands(Console &console, gameboy &gbInst) {
    console.addCommand("poke", [&](std::vector<std::string> args) {
        if (args.size() == 2) {
            gbInst.override_ram(ConsoleCmd::toInt<uint32_t>(args[0]), (uint8_t) ConsoleCmd::toInt<uint8_t>(args[1]));
        } else {
            console.addError("Usage: poke [address] [value]");
        }
    });

    console.addCommand("peek", [&](std::vector<std::string> args) {
        if (args.size() == 1) {
            uint32_t address = ConsoleCmd::toInt<uint32_t>(args[0]);
            uint8_t value = gbInst.read_ram<uint8_t>(address);
            std::stringstream s;
            s << "0x" << std::hex << address << ": " << std::dec << (uint32_t) value << " (0x" << std::hex << (uint32_t) value << std::dec << ")";
            console.addOutput(s.str());
        } else {
            console.addError("Usage: peek [address]");
        }
    });
}