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
#include "scripting/lua_script_vm.h"

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

void printScanResults(address_scan_result result, Console &console, address_scan_result &last_result, int scanThreshold);

void print_scan_state(Console &console, address_scan_state<uint8_t> &state, std::string searchType);

int main(int argc, char *argv[]) {

    if (argc == 1) {
        std::cerr << "Usage: " << argv[0] << " rom [-s|-m|-c client-address]" << std::endl;
        return 0;
    }

    sdl_gamepad_source gp_source;
    std::unique_ptr<link_cable_source> cable_source{provideLinkCableSource(&argc, &argv)};

    script_manager scriptManager;

    Console console{[&](std::string &command, std::vector<std::string> &args) -> bool {
        return scriptManager.handleUnhandledCommand(command, args);
    }};


    sdl_renderer render{[&]() {
        scriptManager.tick();
        console.draw(SDL_GetVideoSurface());
    }};
    gameboy gbInst{&render, &gp_source, cable_source.get()};

    RomFile romFile{argv[0]};

    file_buffer &romBuffer = romFile.rom();
    file_buffer &saveBuffer = romFile.sram();

    gbInst.load_rom(romBuffer, romBuffer.length(), saveBuffer, saveBuffer.length());

    loadState(render, gbInst, romFile);

    bool endGame = false;
    bool fast_forward = false;

    script_context context{&render, &gp_source, &gbInst};

    console.addCommand(new ConsoleCmd{"poke", [&](std::vector<std::string> args) {
        if (args.size() == 2) {
            gbInst.override_ram(ConsoleCmd::toInt<uint32_t>(args[0]), (uint8_t) ConsoleCmd::toInt<uint8_t>(args[1]));
        } else {
            console.addError("Usage: poke [address] [value]");
        }
    }});

    console.addCommand(new ConsoleCmd{"peek", [&](std::vector<std::string> args) {
        if (args.size() == 1) {

            uint32_t address = ConsoleCmd::toInt<uint32_t>(args[0]);
            uint8_t value = gbInst.read_ram<uint8_t>(address);
            std::stringstream s;
            s << "0x" << std::hex << address << ": " << std::dec << (uint32_t)value <<  " (0x" << std::hex << (uint32_t)value << std::dec << ")";
            console.addOutput(s.str());
        } else {
            console.addError("Usage: peek [address]");
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

    address_scan_state<uint8_t> state;

    console.addCommand(new ConsoleCmd{"start_search", [&](std::vector<std::string> args) {
        state = gbInst.initial_state<uint8_t>();
    }});

    console.addCommand(new ConsoleCmd{"search_greater", [&](std::vector<std::string> args) {
        if (state.size() == 0) {
            console.addOutput("No initial search state, creating now");
            state = gbInst.initial_state<uint8_t>();
        } else {
            state = gbInst.search_greater(state);
            if (state.size() <= scanThreshold) {
                print_scan_state(console, state, "Greater values");
            } else {
                console.addOutput("Greater values: " + std::to_string(state.size()));
            }
        }
    }});

    console.addCommand(new ConsoleCmd{"search_lesser", [&](std::vector<std::string> args) {
        if (state.size() == 0) {
            console.addOutput("No initial search state, creating now");
            state = gbInst.initial_state<uint8_t>();
        } else {
            state = gbInst.search_lesser(state);
            if (state.size() <= scanThreshold) {

                print_scan_state(console, state, "Lesser values");
            } else {
                console.addOutput("Lesser values: " + std::to_string(state.size()));
            }
        }
    }});

    console.addCommand(new ConsoleCmd{"search_changed", [&](std::vector<std::string> args) {
        if (state.size() == 0) {
            console.addOutput("No initial search state, creating now");
            state = gbInst.initial_state<uint8_t>();
        } else {
            state = gbInst.search_changed(state);
            if (state.size() <= scanThreshold) {

                print_scan_state(console, state, "Changed values");
            } else {
                console.addOutput("Changed values: " + std::to_string(state.size()));
            }
        }
    }});

    console.addCommand(new ConsoleCmd{"search_unchanged", [&](std::vector<std::string> args) {
        if (state.size() == 0) {
            console.addOutput("No initial search state, creating now");
            state = gbInst.initial_state<uint8_t>();
        } else {
            state = gbInst.search_unchanged(state);
            if (state.size() <= scanThreshold) {
                print_scan_state(console, state, "Unchanged values");
            } else {
                console.addOutput("Unchanged values: " + std::to_string(state.size()));
            }
        }
    }
    });

    console.addCommand(new ConsoleCmd{"scan_threshold", [&](std::vector<std::string> args) {
        if (args.size() == 0) {
            console.addOutput("Scan threshold: " + std::to_string(scanThreshold));
        } else {
            scanThreshold = ConsoleCmd::toInt<int>(args[0]);
            console.addOutput("Scan threshold now: " + std::to_string(scanThreshold));
        }
    }});

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
                wren_script_vm *wrenVm = new wren_script_vm(context);
                wrenVm->loadScript(file_buffer{file});
                scriptManager.remove_vm(file);
                scriptManager.add_vm(file, wrenVm);
                console.addOutput("Loaded " + file + " wren script");
            } else if (file.find(".lua") != std::string::npos) {
                lua_script_vm *luaVm = new lua_script_vm(context);
                luaVm->loadScript((file_buffer{file}));
                scriptManager.remove_vm(file);
                scriptManager.add_vm(file, luaVm);
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
            scriptManager.remove_vm(args[0]);
        } else {
            console.addError("Usage: unload_script [loaded script file]");
        }
    }});


    limitter frameLimitter{[&] {
        gbInst.tick();
    }};

    SDL_Event event;

    std::map<SDLKey, std::function<void()>> uiActions{
            {SDLK_F5,        [&] {
                saveState(render, gbInst, romFile);
            }},
            {SDLK_F7,        [&] {
                loadState(render, gbInst, romFile);
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

void print_scan_state(Console &console, address_scan_state<uint8_t> &state, std::string searchType) {
    console.addOutput(searchType);
    for (auto &values : state.values()) {
        std::stringstream s;
        s << std::hex << (unsigned int) values.first << ": " << std::dec << (unsigned int) values.second << " (" << std::hex << (unsigned int) values.second << ")";
        console.addOutput(s.str());
    }
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

void printScanResults(address_scan_result result, Console &console, address_scan_result &last_result, int scanThreshold) {
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