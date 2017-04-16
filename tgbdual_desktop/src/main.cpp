/*--------------------------------------------------
   TGB Dual - Gameboy Emulator -
   Copyright (C) 2001  Hii

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <SDL/SDL.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#include <gameboy.h>

#include <script_manager.h>
#include <scripting/wren_script_vm.h>

#include "console/console.h"
#include "emulator_time.h"

#include <input/sdl/sdl_gamepad_source.h>
#include <limitter.h>

#include <io/rom_file.h>
#include <io/file_buffer.h>
#include <io/memory_buffer.h>

#include <linkcable/link_cable_source_provider.h>

#include <commands/scan_commands.h>
#include <commands/script_commands.h>
#include <commands/memory_commands.h>
#include <commands/gameboy_commands.h>

#include <rendering/sdl/sdl_video_renderer.h>
#include <rendering/sdl/sdl_audio_renderer.h>
#include <rendering/gb_video_renderer.h>
#include <rendering/gb_osd_renderer.h>
#include <rendering/gb_audio_renderer.h>
#include <input/gb_gamepad_source.h>
#include <input/gb_sys_command_source.h>
#include <input/sdl/sdl_console_driver.h>
#include <input/gb_console_driver.h>

void loop(console &c, bool &endGame, limitter &frameLimitter, gb_sys_command_source&, gb_console_driver&);

void saveState(gameboy &gbInst, rom_file &romFile) {
    memory_buffer buffer;

    gbInst.save_state([&](uint32_t length) {
        buffer.alloc(length);
        return (uint8_t *) buffer;
    });

    romFile.writeState(buffer, buffer.length());
}

void loadState(gameboy &gbInst, rom_file &romFile) {
    gbInst.load_state(romFile.state());
}

int main(int argc, char *argv[]) {

    if (argc == 1) {
        std::cerr << "Usage: " << argv[0] << " rom [-s|-m|-c client-address]" << std::endl;
        return 0;
    }

    emulator_time::set_time_provider(&SDL_GetTicks);
    emulator_time::set_sleep_provider(&SDL_Delay);

    std::unique_ptr<link_cable_source> cable_source{provideLinkCableSource(&argc, &argv)};

    script_manager scriptManager;

    SDL_InitSubSystem(SDL_INIT_VIDEO);

    SDL_Surface *screen = SDL_SetVideoMode(320 + 200, 288 + 200, 16, SDL_SWSURFACE);
    sdl_video_renderer sdl_video{screen};
    sdl_audio_renderer sdl_audio;

    console cons{&sdl_video, 520, 488/2, std::bind(&script_manager::handleUnhandledCommand, &scriptManager, std::placeholders::_1, std::placeholders::_2), &emulator_time::current_time};

    gb_osd_renderer osdRenderer{&sdl_video};
    gb_video_renderer video_renderer{&sdl_video, [&]() {
        scriptManager.tick();
        osdRenderer.render();
        cons.draw();
    }, 100};

    gb_audio_renderer gb_audio{&sdl_audio};
    sdl_gamepad_source sdl_input;
    gb_gamepad_source gp_source{&sdl_input};

    sdl_console_driver sdl_consoleDriver;
    gb_console_driver consoleDriver{cons, &sdl_consoleDriver, std::bind(&gb_gamepad_source::enable, &gp_source)};

    gameboy gbInst{&video_renderer, &gb_audio, &gp_source, cable_source.get()};
    scan_engine scanEngine{gbInst.createAddressScanner(), std::bind(&console::addOutput, &cons, "Initial search state created")};

    rom_file romFile{argv[0]};

    file_buffer &romBuffer = romFile.rom();
    file_buffer &saveBuffer = romFile.sram();

    gbInst.load_rom(romBuffer, romBuffer.length(), saveBuffer, saveBuffer.length());
    loadState(gbInst, romFile);

    script_context context{&osdRenderer, &gp_source, &gbInst, [&](const std::string &name, script_context::script_command command) {
        cons.removeCommand(name);
        cons.addCommand(name, [command](std::vector<std::string> args) {
            command(args);
        });
    }};

    registerMemoryCommands(cons, gbInst);
    registerScanCommands(cons, scanEngine);
    registerScriptCommands(scriptManager, cons, context);
    registerGameBoyCommands(cons, gbInst, romFile);

    bool endGame = false;
    cons.addCommand("quit", [&](std::vector<std::string> args) {
        endGame = true;
    });

    limitter frameLimitter{std::bind(&gameboy::tick, &gbInst)};

    gb_sys_command_source sys_command_source{ &sdl_input,
            [&] {
                saveState(gbInst, romFile);
                osdRenderer.display_message("State saved", 2000);
            }, [&] {
                loadState(gbInst, romFile);
                osdRenderer.display_message("State loaded", 2000);
            }, [&] {
                static bool fast_forward = false;
                fast_forward = !fast_forward;
                if (fast_forward) {
                    gbInst.set_speed(9);
                    osdRenderer.display_message("Fast forward enabled", 2000);
                    frameLimitter.fast();
                } else {
                    gbInst.set_speed(0);
                    osdRenderer.display_message("Fast forward disabled", 2000);
                    frameLimitter.normal();
                }
            }, [&] {
                endGame = true;
            }, [&] {
                scriptManager.activate();
            }, [&] {
                cons.open();
                gp_source.reset_pad();
                gp_source.disable();
            }
    };

    loop(cons, endGame, frameLimitter, sys_command_source, consoleDriver);

    gbInst.save_sram(std::bind(&rom_file::writeSram, &romFile, std::placeholders::_1, std::placeholders::_2));

    SDL_QuitSubSystem(SDL_INIT_VIDEO);

    return 0;
}

void loop(console &c, bool &endGame, limitter &frameLimitter, gb_sys_command_source &sys_command_source, gb_console_driver &consoleDriver) {
    while (!endGame) {
        if (!c.isOpen()) {
            sys_command_source.update();
        } else {
            consoleDriver.update();
        }
        frameLimitter.limit();
    }
}