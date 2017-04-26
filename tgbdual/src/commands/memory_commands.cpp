#include <commands/memory_commands.h>

void registerMemoryCommands(tgbdual &tgb) {
    tgb.addConsoleCommand("poke", [&](std::vector<std::string> args) {
        if (args.size() == 2) {
            tgb.writeRam(console_cmd::toInt<uint32_t>(args[0]), console_cmd::toInt<uint8_t>(args[1]));
        } else {
            tgb.addConsoleErrorOutput("Usage: poke [address] [value]");
        }
    });

    tgb.addConsoleCommand("peek", [&](std::vector<std::string> args) {
        if (args.size() == 1) {
            auto address = console_cmd::toInt<uint32_t>(args[0]);
            auto value = tgb.readRam<uint8_t>(address);
            std::stringstream s;
            s << "0x" << std::hex << address << ": " << std::dec << (uint32_t) value << " (0x" << std::hex << (uint32_t) value << std::dec << ")";
            tgb.addConsoleOutput(s.str());
        } else {
            tgb.addConsoleErrorOutput("Usage: peek [address]");
        }
    });
}