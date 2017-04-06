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

#include <SDL.h>
#include <iostream>
#include <getopt.h>

#include <sys/stat.h>
#include <vector>

#include <gameboy.h>

#include "io/file_buffer.h"
#include "io/memory_buffer.h"

#include <fstream>

#include "sdl_renderer.h"

#include "network/null_link_source.h"

#include "network/multicast_transmitter.h"

#include <network/tcp_client.h>
#include <network/tcp_server.h>
#include <scripting/wren_macro_runner.h>
#include <console/Console.h>

#include "input/sdl_gamepad_source.h"
#include "scripting/lua_macro_runner.h"
#include "limitter.h"

link_cable_source *processArguments(int *argc, char ***argv) {
    int option = 0;
    link_cable_source *selected_cable_source = new null_link_source();
    while ((option = getopt(*argc, *argv, "smc:")) != -1) {
        switch (option) {
            case 's': {
                selected_cable_source = new tcp_server();
            }
                break;
            case 'm': {
                cout << "Broadcasting availability as client" << endl;
                multicast_transmitter mc_transmitter{"239.0.10.0", 1337};
                mc_transmitter.transcieve([&](std::string addr) {
                    std::cout << "Should connect to " << addr << std::endl;
                    selected_cable_source = new tcp_client(addr);
                });
            }
                break;
            case 'c':
                selected_cable_source = new tcp_client(optarg);
                break;
            default:
            case '?':
                if (optopt == 'c') {
                    cerr << "Target address must be passed in with -c" << endl;
                } else {
                    cerr << "Unknown option " << optopt << endl;
                }
                break;
        }
    }

    (*argc) -= optind;
    (*argv) += optind;

    return selected_cable_source;
}

class RomFile {
public:
    RomFile(const std::string &romPath);

    file_buffer &rom();

    file_buffer &sram();

    file_buffer &state();

    void writeSram(uint8_t *sramData, uint32_t length);

    void writeState(uint8_t *stateData, uint32_t length);

private:
    std::string _romPath;
    std::string _sramPath;
    std::string _statePath;

    file_buffer _romBuffer;
    file_buffer _sramBuffer;
    file_buffer _stateBuffer;
};

RomFile::RomFile(const std::string &romPath) :
        _romPath{romPath},
        _sramPath{romPath.substr(0, romPath.find_last_of(".")) + ".sav"},
        _statePath{romPath.substr(0, romPath.find_last_of(".")) + ".sv0"},
        _romBuffer{_romPath},
        _sramBuffer{_sramPath},
        _stateBuffer{_statePath} {
    struct stat st;
    if (stat(_romPath.c_str(), &st) != 0) {
        throw std::domain_error("Failed to open rom: " + _romPath);
    }
}

file_buffer &RomFile::rom() {
    return _romBuffer;
}

file_buffer &RomFile::sram() {
    return _sramBuffer;
}

file_buffer &RomFile::state() {
    return _stateBuffer;
}

void RomFile::writeSram(uint8_t *sramData, uint32_t length) {
    std::fstream sram{_sramPath, std::ios::out};
    sram.write((const char *) sramData, length);
    sram.close();

    _sramBuffer = file_buffer{_sramPath};
}

void RomFile::writeState(uint8_t *stateData, uint32_t length) {
    std::fstream state{_statePath, std::ios::out};
    state.write((const char *) stateData, length);
    state.close();

    _stateBuffer = file_buffer{_statePath};
}

void loadState(sdl_renderer &render, gameboy &gbInst, RomFile &stateFile);

void saveState(sdl_renderer &render, gameboy &gbInst, RomFile &rom);

void searchAddress8bit(sdl_renderer &render, gameboy &gbInst, uint32_t search_value, address_scan_result &last_result);

void searchAddress16bit(sdl_renderer &render, gameboy &gbInst, uint32_t search_value, address_scan_result &last_result);

void searchAddress32bit(sdl_renderer &render, gameboy &gbInst, uint32_t search_value, address_scan_result &last_result);

void fastForwardSpeed(sdl_renderer &render, gameboy &gbInst, limitter &frameLimitter);

void normalSpeed(sdl_renderer &render, gameboy &gbInst, limitter &frameLimitter);

void displaySearchResults(sdl_renderer &render, address_scan_result &last_result, address_scan_result &result);


int main(int argc, char *argv[]) {

    if (argc == 1) {
        cerr << "Usage: " << argv[0] << " rom [-s|-m|-c client-address]" << endl;
        return 0;
    }

    sdl_renderer render;
    sdl_gamepad_source gp_source;
    link_cable_source *cable_source = processArguments(&argc, &argv);

    gameboy gbInst{&render, &gp_source, cable_source};

    script_context context{&render, &gp_source, &gbInst};

    RomFile romFile{argv[0]};

    file_buffer &romBuffer = romFile.rom();
    file_buffer &saveBuffer = romFile.sram();

    gbInst.load_rom(romBuffer, romBuffer.length(), saveBuffer, saveBuffer.length());

    SDL_Event e;
    bool endGame = false;
    bool fast_forward = false;
    Console console;

    console.addCommand(new ConsoleCmd{"poke", [&](std::vector<std::string> args) {
        uint32_t address = 0;
        uint8_t value = 0;
        address = ConsoleCmd::toInt(args[0]);
        value = ConsoleCmd::toInt(args[1]);

        gbInst.override_ram(address, (uint8_t) value);
    }});

    address_scan_result last_result{{}};
    int scanThreshold = 3;
    console.addCommand(new ConsoleCmd{"scan", [&](std::vector<std::string> args) {

        if (args.size() != 1) {
            console.addError("Usage: scan [value]");
            return;
        }

        int value = ConsoleCmd::toInt(args[0]);

        if (value <= 255) {
            address_scan_result result = gbInst.scan_for_address((uint8_t) value);
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
        } else if (value <= (0x1 << 16)) {
            address_scan_result result = gbInst.scan_for_address((uint16_t) value);
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
        } else if (value <= (0x1 << 31)) {
            address_scan_result result = gbInst.scan_for_address((uint32_t) value);
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
    }});

    console.addCommand(new ConsoleCmd{"scan_threshold", [&](std::vector<std::string> args) {

        if (args.size() == 0) {
            console.addOutput("Scan threshold: " + std::to_string(scanThreshold));
        } else {
            scanThreshold = ConsoleCmd::toInt(args[0]);
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
                console.addError(file +" is not loaded");
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


    while (!endGame) {
        while (SDL_PollEvent(&e)) {
            if (!console.isOpen()) {
                gp_source.update_pad_state(e);
            }

            if (e.type == SDL_QUIT) {
                endGame = true;
            } else if (e.type == SDL_KEYDOWN) {
                auto sym = e.key.keysym.sym;
                if (console.isOpen()) {
                    if (sym == SDLK_BACKQUOTE) {
                        console.close();
                    } else {
                        console.update(sym, e.key.keysym.mod);
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
