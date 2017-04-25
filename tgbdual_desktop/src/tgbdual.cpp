#include <io/rom_file.h>
#include <io/memory_buffer.h>
#include "tgbdual.h"
#include "emulator_time.h"

tgbdual::tgbdual(core_services *services, link_cable_source *cableSource, char *romFilePath) :
        _console{services->videoRenderer(), 520, 488 / 2, std::bind(&script_manager::handleUnhandledCommand, &_scriptManager, std::placeholders::_1, std::placeholders::_2),
                 &emulator_time::current_time},
        _osdRenderer{services->videoRenderer()},
        _consoleDriver{_console, services->consoleDriver(), [&] { _gameboy.enableInput(); }},
        _gamepadSource{services->gamepadSource()},
        _sysCommandSource{services->sysCommandSource(),
                          [&] {
                              saveState();
                          },
                          [&] {
                              loadState();
                          },
                          [&] {
                              static bool fast_forward = false;
                              fast_forward = !fast_forward;
                              if (fast_forward) {
                                  _gameboy.set_speed(9);
                                  _osdRenderer.display_message("Fast forward enabled", 2000);
                                  _frameLimitter.fast();
                              } else {
                                  _gameboy.set_speed(0);
                                  _osdRenderer.display_message("Fast forward disabled", 2000);
                                  _frameLimitter.normal();
                              }
                          },
                          [&] {
                              _alive = false;
                          },
                          std::bind(&script_manager::activate, &_scriptManager),
                          [&] {
                              _console.open();
                              _gameboy.disableInput();
                          }
        },
        _gameboy{services, [&] {
            _scriptManager.tick();
            _osdRenderer.render();
            _console.draw();
        }, cableSource},
        _memoryBridge{_gameboy},
        _scriptContext{&_osdRenderer, &_gamepadSource, &_memoryBridge, [&](const std::string &name, script_context::script_command command) {
            _console.removeCommand(name);
            _console.addCommand(name, command);
        }},
        _romFile{romFilePath} {
    file_buffer &romBuffer = _romFile.rom();
    file_buffer &saveBuffer = _romFile.sram();

    _gameboy.load_rom(romBuffer, romBuffer.length(), saveBuffer, saveBuffer.length());
    loadState();
}

gameboy &tgbdual::getGameboy() {
    return _gameboy;
}

console &tgbdual::getConsole() {
    return _console;
}

script_manager &tgbdual::getScriptManager() {
    return _scriptManager;
}

script_services *tgbdual::getScriptServices() {
    return &_scriptContext;
}

void tgbdual::tick() {
    if (!_console.isOpen()) {
        _sysCommandSource.update();
    } else {
        _consoleDriver.update();
    }
    _gameboy.tick();
}

void tgbdual::loadState() {
    _gameboy.load_state(_romFile.state());
    _osdRenderer.display_message("State loaded", 3000);
}

void tgbdual::saveState() {
    memory_buffer buffer;

    _gameboy.save_state([&](uint32_t length) {
        buffer.alloc(length);
        return (uint8_t *) buffer;
    });

    _romFile.writeState(buffer, buffer.length());
    _osdRenderer.display_message("State saved", 3000);
}

void tgbdual::saveSram() {
    _gameboy.save_sram(std::bind(&rom_file::writeSram, &_romFile, std::placeholders::_1, std::placeholders::_2));
}

bool tgbdual::limit() {
    _frameLimitter.limit();
    return _alive;
}

void tgbdual::quit() {
    _alive = false;
}



