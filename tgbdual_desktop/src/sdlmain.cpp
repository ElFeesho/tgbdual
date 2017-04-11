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

#include <gameboy.h>
#include <script_manager.h>

#include "scripting/wren_script_vm.h"

#include "console/Console.h"

#include "io/file_buffer.h"
#include "io/memory_buffer.h"

#include "sdl_renderer.h"

#include "input/sdl_gamepad_source.h"
#include "limitter.h"

#include "RomFile.h"

#include "link_cable_source_provider.h"
#include "scan_engine.h"

#include <commands/scan_commands.h>
#include <commands/script_commands.h>
#include <commands/memory_commands.h>
#include <commands/gameboy_commands.h>

void saveState(gameboy &gbInst, RomFile &romFile) {
    memory_buffer buffer;

    gbInst.save_state([&](uint32_t length) {
        buffer.alloc(length);
        return (uint8_t *) buffer;
    });

    romFile.writeState(buffer, buffer.length());
}

void loadState(gameboy &gbInst, RomFile &romFile) {
    gbInst.load_state(romFile.state());
}

void normalSpeed(sdl_renderer &render, gameboy &gbInst, limitter &frameLimitter) {
    gbInst.set_speed(0);
    render.display_message("Fast forward disabled", 2000);
    frameLimitter.normal();
}

void fastForwardSpeed(sdl_renderer &render, gameboy &gbInst, limitter &frameLimitter) {
    gbInst.set_speed(9);
    render.display_message("Fast forward enabled", 2000);
    frameLimitter.fast();
}


int main(int argc, char *argv[]) {

    if (argc == 1) {
        std::cerr << "Usage: " << argv[0] << " rom [-s|-m|-c client-address]" << std::endl;
        return 0;
    }

    std::unique_ptr<link_cable_source> cable_source{provideLinkCableSource(&argc, &argv)};

    script_manager scriptManager;

    Console console{[&](std::string &command, std::vector<std::string> &args) -> bool {
        return scriptManager.handleUnhandledCommand(command, args);
    }};

    sdl_renderer render{[&]() {
        scriptManager.tick();
        console.draw(SDL_GetVideoSurface());
    }};

    sdl_gamepad_source gp_source;
    gameboy gbInst{&render, &gp_source, cable_source.get()};
    scan_engine scanEngine{gbInst.createAddressScanner(), [&] {
        console.addOutput("Initial search state created");
    }};

    RomFile romFile{argv[0]};

    file_buffer &romBuffer = romFile.rom();
    file_buffer &saveBuffer = romFile.sram();

    gbInst.load_rom(romBuffer, romBuffer.length(), saveBuffer, saveBuffer.length());
    loadState(gbInst, romFile);

    bool endGame = false;
    bool fast_forward = false;

    script_context context{&render, &gp_source, &gbInst};

    registerMemoryCommands(console, gbInst);
    registerScanCommands(console, scanEngine);
    registerScriptCommands(scriptManager, console, context);
    registerGameBoyCommands(console, gbInst, romFile);

    console.addCommand("quit", [&](std::vector<std::string> args) {
        endGame = true;
    });

    limitter frameLimitter{[&] {
        gbInst.tick();
    }};

    SDL_Event event;

    std::map<SDLKey, std::function<void()>> uiActions{
            {SDLK_F5,        [&] {
                saveState(gbInst, romFile);
                render.display_message("State saved", 2000);
            }},
            {SDLK_F7,        [&] {
                loadState(gbInst, romFile);
                render.display_message("State loaded", 2000);
            }},
            {SDLK_ESCAPE,    [&] {
                endGame = true;
            }},
            {SDLK_SPACE,     [&] {
                scriptManager.activate();
            }},
            {SDLK_TAB,       [&] {
                fast_forward = !fast_forward;
                if (fast_forward) {
                    fastForwardSpeed(render, gbInst, frameLimitter);
                } else {
                    normalSpeed(render, gbInst, frameLimitter);
                }
            }},
            {SDLK_BACKQUOTE, [&] {
                console.open();
                gp_source.reset_pad();
            }}
    };

    while (!endGame) {
        while (SDL_PollEvent(&event)) {

            if (!console.isOpen()) {
                gp_source.update_pad_state(event);
            }

            if (event.type == SDL_QUIT) {
                endGame = true;
            } else if (event.type == SDL_KEYDOWN) {
                auto sym = event.key.keysym.sym;
                if (console.isOpen()) {
                    if (sym == SDLK_BACKQUOTE) {
                        console.close();
                    } else {
                        console.update(sym, event.key.keysym.mod);
                    }
                } else {
                    if (uiActions.find(sym) != uiActions.end()) {
                        uiActions[sym]();
                    }
                }
            }
        }
        frameLimitter.limit();
    }

    gbInst.save_sram([&](uint8_t *sramData, uint32_t len) {
        romFile.writeSram(sramData, len);
    });

    SDL_Quit();

    return 0;
}