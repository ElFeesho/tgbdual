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
}