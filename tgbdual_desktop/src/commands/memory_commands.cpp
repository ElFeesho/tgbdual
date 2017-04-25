#include <tgbdual.h>
#include "memory_commands.h"

void registerMemoryCommands(tgbdual &tgb) {
    tgb.getConsole().addCommand("poke", [&](std::vector<std::string> args) {
        if (args.size() == 2) {
            tgb.getGameboy().override_ram(console_cmd::toInt<uint32_t>(args[0]), (uint8_t) console_cmd::toInt<uint8_t>(args[1]));
        } else {
            tgb.getConsole().addError("Usage: poke [address] [value]");
        }
    });

    tgb.getConsole().addCommand("peek", [&](std::vector<std::string> args) {
        if (args.size() == 1) {
            uint32_t address = console_cmd::toInt<uint32_t>(args[0]);
            uint8_t value = tgb.getGameboy().read_ram<uint8_t>(address);
            std::stringstream s;
            s << "0x" << std::hex << address << ": " << std::dec << (uint32_t) value << " (0x" << std::hex << (uint32_t) value << std::dec << ")";
            tgb.getConsole().addOutput(s.str());
        } else {
            tgb.getConsole().addError("Usage: peek [address]");
        }
    });
}