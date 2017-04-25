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

    gameboy &getGameboy();
    console &getConsole();
    script_manager &getScriptManager();
    script_services *getScriptServices();

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