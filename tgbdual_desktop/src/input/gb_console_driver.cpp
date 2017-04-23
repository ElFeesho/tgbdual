#include "gb_console_driver.h"

gb_console_driver::gb_console_driver(console &consoleToDrive, tgb::console_driver *consoleDriver, console_close_cb consoleCloseCb) : _console{consoleToDrive}, _consoleDriver{consoleDriver}, _consoleCloseCb{consoleCloseCb} {

}

void gb_console_driver::update() {

    _consoleDriver->update(
            [&](char key) {
                _console.key_down(key);
            },
            [&](char key) {
                _console.key_up();
            },
            [&](tgb::console_driver::CommandKey key) {
                if (key == tgb::console_driver::CommandKey::CLOSE_CONSOLE) {
                    _console.close();
                    _consoleCloseCb();
                } else {
                    _console.key_down(commandKeyMap[key]);
                }
            },
            [&](tgb::console_driver::CommandKey key) {
                _console.key_up();
            });
}
