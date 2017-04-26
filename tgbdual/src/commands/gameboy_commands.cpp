#include "gameboy_commands.h"

void registerGameBoyCommands(tgbdual &tgb) {
    tgb.addConsoleCommand("save", [&](std::vector<std::string> args) {
        tgb.saveState();
        tgb.addConsoleOutput("State saved");
    });

    tgb.addConsoleCommand("load", [&](std::vector<std::string> args) {
        tgb.loadState();
        tgb.addConsoleOutput("State loaded");
    });

    tgb.addConsoleCommand("cheat", [&](std::vector<std::string> args) {
        if (args.empty()) {
            tgb.addConsoleErrorOutput("Usage: cheat [game genie code]");
        } else {
            tgb.addCheat(args[0]);
            tgb.addConsoleOutput("Cheat Added");
        }
    });
}