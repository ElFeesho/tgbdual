#include "gameboy_commands.h"

void registerGameBoyCommands(tgbdual &tgb) {
    tgb.getConsole().addCommand("save", [&](std::vector<std::string> args) {
        tgb.saveState();
        tgb.getConsole().addOutput("State saved");
    });

    tgb.getConsole().addCommand("load", [&](std::vector<std::string> args) {
        tgb.loadState();
        tgb.getConsole().addOutput("State loaded");
    });
}