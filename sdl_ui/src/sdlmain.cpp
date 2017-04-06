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
#include <fstream>
#include <vector>

#include <getopt.h>

#include <gameboy.h>

#include "network/tcp_client.h"
#include "network/tcp_server.h"
#include "network/null_link_source.h"
#include "network/multicast_transmitter.h"

#include "scripting/wren_macro_runner.h"
#include "scripting/lua_macro_runner.h"

#include "console/Console.h"

#include "io/file_buffer.h"
#include "io/memory_buffer.h"

#include "sdl_renderer.h"

#include "input/sdl_gamepad_source.h"
#include "limitter.h"

#include "RomFile.h"

#include "link_cable_source_provider.h"


void loadState(sdl_renderer &render, gameboy &gbInst, RomFile &stateFile);

void saveState(sdl_renderer &render, gameboy &gbInst, RomFile &rom);

void fastForwardSpeed(sdl_renderer &render, gameboy &gbInst, limitter &frameLimitter);

void normalSpeed(sdl_renderer &render, gameboy &gbInst, limitter &frameLimitter);

void printScanResults(address_scan_result result, Console &console, address_scan_result &last_result, int scanThreshold)
;

int main(int argc, char *argv[]) {

    if (argc == 1) {
        std::cerr << "Usage: " << argv[0] << " rom [-s|-m|-c client-address]" << std::endl;
        return 0;
    }

    sdl_renderer render;
    sdl_gamepad_source gp_source;
    std::unique_ptr<link_cable_source> cable_source{provideLinkCableSource(&argc, &argv)};

    gameboy gbInst{&render, &gp_source, cable_source.get()};

    script_context context{&render, &gp_source, &gbInst};

    RomFile romFile{argv[0]};

    file_buffer &romBuffer = romFile.rom();
    file_buffer &saveBuffer = romFile.sram();

    gbInst.load_rom(romBuffer, romBuffer.length(), saveBuffer, saveBuffer.length());

    bool endGame = false;
    bool fast_forward = false;
    Console console;

    console.addCommand(new ConsoleCmd{"poke", [&](std::vector<std::string> args) {
        if (args.size() == 2) {
            gbInst.override_ram(ConsoleCmd::toInt<uint32_t>(args[0]), (uint8_t) ConsoleCmd::toInt<uint8_t>(args[1]));
        } else {
            console.addError("Usage: poke [address] [value]");
        }
    }});

    address_scan_result last_result{{}};
    int scanThreshold = 3;
    console.addCommand(new ConsoleCmd{"scan", [&](std::vector<std::string> args) {

        if (args.size() != 1) {
            console.addError("Usage: scan [value]");
            return;
        }

        int value = ConsoleCmd::toInt<int>(args[0]);

        if (value <= 255) {
            printScanResults(gbInst.scan_for_address((uint8_t) value), console, last_result, scanThreshold);
        } else if (value <= (0x1 << 16)) {
            printScanResults(gbInst.scan_for_address((uint16_t) value), console, last_result, scanThreshold);
        } else if (value <= (0x1 << 31)) {
            printScanResults(gbInst.scan_for_address((uint32_t) value), console, last_result, scanThreshold);
        }
    }});

    console.addCommand(new ConsoleCmd{"scan_threshold", [&](std::vector<std::string> args) {
        if (args.size() == 0) {
            console.addOutput("Scan threshold: " + std::to_string(scanThreshold));
        } else {
            scanThreshold = ConsoleCmd::toInt<int>(args[0]);
            console.addOutput("Scan threshold now: " + std::to_string(scanThreshold));
        }
    }});

    std::map<std::string, std::unique_ptr<macro_runner>> scriptVms;

    console.addCommand(new ConsoleCmd{"clear_scan", [&](std::vector<std::string> args) {
        last_result = address_scan_result{{}};
        console.addOutput("Cleared previous search results");
    }});

    console.addCommand(new ConsoleCmd{"quit", [&](std::vector<std::string> args) {
        endGame = true;
    }});

    console.addCommand(new ConsoleCmd{"save", [&](std::vector<std::string> args) {
        saveState(render, gbInst, romFile);
    }});

    console.addCommand(new ConsoleCmd{"load", [&](std::vector<std::string> args) {
        loadState(render, gbInst, romFile);
    }});

    console.addCommand(new ConsoleCmd{"load_script", [&](std::vector<std::string> args) {
        if (args.size() == 1) {
            std::string &file = args[0];
            if (file.find(".wren") != std::string::npos) {
                wren_macro_runner *wrenVm = new wren_macro_runner(context);
                wrenVm->loadScript(file_buffer{file});
                if (scriptVms.find(file) != scriptVms.end()) {
                    scriptVms.erase(scriptVms.find(file));
                }
                scriptVms.emplace(file, std::unique_ptr<macro_runner>(wrenVm));
                console.addOutput("Loaded " + file + " wren script");
            } else if (file.find(".lua") != std::string::npos) {
                lua_macro_runner *luaVm = new lua_macro_runner(context);
                luaVm->loadScript((file_buffer{file}));
                if (scriptVms.find(file) != scriptVms.end()) {
                    scriptVms.erase(scriptVms.find(file));
                }
                scriptVms.emplace(file, std::unique_ptr<macro_runner>(luaVm));
                console.addOutput("Loaded " + file + " lua script");
            } else {
                console.addError("Cannot load " + file);
            }
        } else {
            console.addError("Usage: load_script [file]");
        }
    }});

    console.addCommand(new ConsoleCmd{"unload_script", [&](std::vector<std::string> args) {
        if (args.size() == 1) {
            std::string &file = args[0];

            if (scriptVms.find(file) != scriptVms.end()) {
                scriptVms.erase(scriptVms.find(file));
            } else {
                console.addError(file + " is not loaded");
            }
        } else {
            console.addError("Usage: unload_script [loaded script file]");
        }
    }});


    SDL_Surface *screen = SDL_GetVideoSurface();
    limitter frameLimitter{[&] {
        for (auto &vm : scriptVms) {
            vm.second->tick();
        }
        gbInst.tick();
        console.draw(screen);
        SDL_Flip(screen);
    }};

    SDL_Event event;
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
                    if (sym == SDLK_BACKQUOTE) {
                        console.open();
                        gp_source.reset_pad();
                    } else if (sym == SDLK_F5) {
                        saveState(render, gbInst, romFile);
                    } else if (sym == SDLK_F7) {
                        loadState(render, gbInst, romFile);
                    } else if (sym == SDLK_ESCAPE) {
                        endGame = true;
                    } else if (sym == SDLK_SPACE) {
                        for (auto &vm : scriptVms) {
                            vm.second->activate();
                        }
                    } else if (sym == SDLK_TAB) {
                        fast_forward = !fast_forward;
                        if (fast_forward) {
                            fastForwardSpeed(render, gbInst, frameLimitter);
                        } else {
                            normalSpeed(render, gbInst, frameLimitter);
                        }
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

void saveState(sdl_renderer &render, gameboy &gbInst, RomFile &romFile) {
    memory_buffer buffer;
    gbInst.save_state([&](uint32_t length) {
        buffer.alloc(length);
        return (uint8_t *) buffer;
    });
    romFile.writeState(buffer, buffer.length());

    render.display_message("State saved", 2000);
}

void loadState(sdl_renderer &render, gameboy &gbInst, RomFile &romFile) {
    gbInst.load_state(romFile.state());
    render.display_message("State loaded", 2000);
}

void printScanResults(address_scan_result result, Console &console, address_scan_result &last_result, int scanThreshold)
{
    if (last_result.size() > 1) {
        result = last_result.mutual_set(result);
    }
    last_result = result;
    console.addOutput("Found " + std::to_string((int) result.size()) + " results");
    if (result.size() <= scanThreshold) {
        for (int i = 0; i < result.size(); i++) {
            std::stringstream s;
            s << std::hex << result[i];
            console.addOutput(std::to_string(i + 1) + ": 0x" + s.str());
        }
    }
}