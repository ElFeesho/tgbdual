#pragma once

#include <map>
#include <console/console.h>
#include "console_driver.h"

class gb_console_driver {
public:
    gb_console_driver(console &consoleToDrive, tgb::console_driver *consoleDriver);

    void update();

private:
    std::map<tgb::console_driver::CommandKey, char> commandKeyMap{
            {tgb::console_driver::CommandKey::UP, 0},
            {tgb::console_driver::CommandKey::DOWN, 1},
            {tgb::console_driver::CommandKey::LEFT, 2},
            {tgb::console_driver::CommandKey::RIGHT, 3},
            {tgb::console_driver::CommandKey::CLOSE_CONSOLE, -1},
            {tgb::console_driver::CommandKey::RETURN, 5},
            {tgb::console_driver::CommandKey::TAB, 6},
            {tgb::console_driver::CommandKey::BACKSPACE, 7},
    };

    console &_console;
    tgb::console_driver *_consoleDriver;
};
