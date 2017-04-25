#pragma once

#include <script_manager.h>
#include <console/console.h>
#include <rendering/gb_osd_renderer.h>
#include <input/gb_gamepad_source.h>
#include <input/gb_console_driver.h>
#include <script_context.h>
#include <gameboy.h>
#include <input/gb_sys_command_source.h>
#include "gameboy_memory_bridge.h"
#include "limitter.h"
#include <io/rom_file.h>

class tgbdual {
public:
    explicit tgbdual(core_services *services, link_cable_source *cableSource, char *romFilePath);

    void addConsoleCommand(const std::string &command, std::function<void(std::vector<std::string>)> &&commandFunc);
    void addConsoleOutput(const std::string &output);
    void addConsoleErrorOutput(const std::string &output);
    address_scanner createAddressScanner();

    void addVm(const std::string &name, script_vm *vm);
    void removeVm(const std::string &name);

    script_services *getScriptServices();

    template<typename T>
    T readRam(uint32_t address) {
        return _gameboy.read_ram<T>(address);
    }

    template<typename T>
    void writeRam(uint32_t address, T value) {
        _gameboy.override_ram(address, value);
    }

    void loadState();
    void saveState();

    void tick();

    void saveSram();

    bool limit();

    void quit();

private:
    script_manager _scriptManager;
    console _console;
    gb_osd_renderer _osdRenderer;
    gb_console_driver _consoleDriver;
    gb_gamepad_source _gamepadSource;
    gb_sys_command_source _sysCommandSource;
    gameboy _gameboy;

    gameboy_memory_bridge _memoryBridge;
    script_context _scriptContext;

    rom_file _romFile;

    limitter _frameLimitter{std::bind(&tgbdual::tick, this)};

    bool _alive{true};
};