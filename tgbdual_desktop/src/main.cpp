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

#include <input/sdl_gamepad_source.h>
#include <limitter.h>

#include <io/rom_file.h>
#include <io/file_buffer.h>
#include <io/memory_buffer.h>

#include <linkcable/link_cable_source_provider.h>

#include <commands/scan_commands.h>
#include <commands/script_commands.h>
#include <commands/memory_commands.h>
#include <commands/gameboy_commands.h>

#include <rendering/sdl_video_renderer.h>
#include <rendering/sdl_audio_renderer.h>
#include <rendering/sdl_osd_renderer.h>

void loop(console &c, sdl_gamepad_source &gp_source, bool &endGame, limitter &frameLimitter, std::map<SDLKey, std::function<void()>> &uiActions);

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

    console console{[&](std::string &command, std::vector<std::string> &args) -> bool {
        return scriptManager.handleUnhandledCommand(command, args);
    }, &emulator_time::current_time};

    SDL_Surface *screen = SDL_SetVideoMode(320 + 200, 288 + 200, 16, SDL_SWSURFACE);
    sdl_osd_renderer osdRenderer{screen};
    sdl_video_renderer video_renderer{screen, 100, [&]() {
        scriptManager.tick();
        osdRenderer.render();
        console.draw(SDL_GetVideoSurface());
        SDL_Flip(SDL_GetVideoSurface());
    }};

    sdl_audio_renderer audio_renderer;

    sdl_gamepad_source gp_source;
    gameboy gbInst{&video_renderer, &audio_renderer, &gp_source, cable_source.get()};
    scan_engine scanEngine{gbInst.createAddressScanner(), [&] {
        console.addOutput("Initial search state created");
    }};

    rom_file romFile{argv[0]};

    file_buffer &romBuffer = romFile.rom();
    file_buffer &saveBuffer = romFile.sram();

    gbInst.load_rom(romBuffer, romBuffer.length(), saveBuffer, saveBuffer.length());
    loadState(gbInst, romFile);

    script_context context{&osdRenderer, &gp_source, &gbInst, [&](const std::string &name, script_context::script_command command) {
        console.removeCommand(name);
        console.addCommand(name, [command](std::vector<std::string> args) {
            command(args);
        });
    }, [&](const std::string &name) {
        //console.removeCommand(name);
    }};

    registerMemoryCommands(console, gbInst);
    registerScanCommands(console, scanEngine);
    registerScriptCommands(scriptManager, console, context);
    registerGameBoyCommands(console, gbInst, romFile);

    bool endGame = false;
    console.addCommand("quit", [&](std::vector<std::string> args) {
        endGame = true;
    });

    limitter frameLimitter{[&] {
        gbInst.tick();
    }};

    std::map<SDLKey, std::function<void()>> uiActions{
            {SDLK_F5,
            [&] {
                saveState(gbInst, romFile);
                osdRenderer.display_message("State saved", 2000);
            }},
            {SDLK_F7,
            [&] {
                loadState(gbInst, romFile);
                osdRenderer.display_message("State loaded", 2000);
            }},
            {SDLK_ESCAPE,
            [&] {
                endGame = true;
            }},
            {SDLK_SPACE,
            [&] {
                scriptManager.activate();
            }},
            {SDLK_TAB,
            [&] {
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
            }},
            {SDLK_BACKQUOTE, [&] {
                console.open();
                gp_source.reset_pad();
            }}
    };

    loop(console, gp_source, endGame, frameLimitter, uiActions);

    gbInst.save_sram([&](uint8_t *sramData, uint32_t len) {
        romFile.writeSram(sramData, len);
    });

    SDL_Quit();

    return 0;
}

void loop(console &c, sdl_gamepad_source &gp_source, bool &endGame, limitter &frameLimitter, std::map<SDLKey, std::function<void()>> &uiActions) {
    SDL_Event event;
    while (!endGame) {
        while (SDL_PollEvent(&event)) {

            if (!c.isOpen()) {
                gp_source.update_pad_state(event);
            }

            if (event.type == SDL_QUIT) {
                endGame = true;
            } else if (event.type == SDL_KEYDOWN) {
                auto sym = event.key.keysym.sym;
                if (c.isOpen()) {
                    if (sym == SDLK_BACKQUOTE) {
                        c.close();
                    } else {
                        c.key_down(sym, event.key.keysym.mod);
                    }
                } else {
                    if (uiActions.find(sym) != uiActions.end()) {
                        uiActions[sym]();
                    }
                }
            }
            else if (event.type == SDL_KEYUP) {
                auto sym = event.key.keysym.sym;
                if (c.isOpen()) {
                    c.key_up(sym, event.key.keysym.mod);
                }
            }
        }
        frameLimitter.limit();
    }
}