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

#include <gameboy.h>

#include "file_buffer.h"
#include "memory_buffer.h"

#include <fstream>
   
#include "sdl_renderer.h"

#include "null_link_source.h"

#include "multicast_transmitter.h"

#include "tcp_client.h"
#include "tcp_server.h"

#include "sdl_gamepad_source.h"
#include "macro_runner.h"


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
                }
                else {
                    cerr << "Unknown option " << optopt << endl;
                }
                break;
        }
    }

    (*argc) -= optind;
    (*argv) += optind;

    return selected_cable_source;
}

void limit(uint32_t targetTime, std::function<void()> operation) {
    uint32_t startTime = SDL_GetTicks();

    operation();

    uint32_t operationTime = SDL_GetTicks() - startTime;

    if (operationTime < targetTime) {
        SDL_Delay(targetTime - operationTime);
    }
}

int main(int argc, char *argv[]) {

    if (argc == 1) {
        cerr << "Usage: " << argv[0] << " rom [-s|-m|-c client-address]" << endl;
        return 0;
    }

    sdl_renderer render;
	macro_runner runner{&render};
    sdl_gamepad_source gp_source;
    link_cable_source *cable_source = processArguments(&argc, &argv);

    std::string romFilename{argv[0]};
    std::string saveFile = romFilename.substr(0, romFilename.find_last_of(".")) + ".sav";
    std::string stateFile = romFilename.substr(0, romFilename.find_last_of(".")) + ".sv0";

    struct stat statBuffer;
    if (stat(romFilename.c_str(), &statBuffer) != 0) {
        cerr << "Rom file " << romFilename << " is not accessible" << endl;
        return -1;
    }

    gameboy gbInst{&render, &gp_source, cable_source};

	file_buffer scriptBuffer{"script.lua"};

	runner.loadScript(scriptBuffer);

    file_buffer romBuffer{romFilename};
    file_buffer saveBuffer{saveFile};
    if (stat(saveFile.c_str(), &statBuffer) != 0) {
        gbInst.load_rom(romBuffer, romBuffer.length(), nullptr, 0);
    }
    else {
        gbInst.load_rom(romBuffer, romBuffer.length(), saveBuffer, saveBuffer.length());
    }

    std::function<void(std::function<void()>)> limitFunc = std::bind(limit, 16, std::placeholders::_1);

    SDL_Event e;
    bool endGame = false;    
    bool fast_forward = false;
    uint32_t search_value = 0;
    
    while (!endGame) {
        while (SDL_PollEvent(&e)) {
            gp_source.update_pad_state(e);

            if (e.type == SDL_QUIT) {
                endGame = true;
            } else if (e.type == SDL_KEYDOWN) {
                auto sym = e.key.keysym.sym;
                if (sym == SDLK_F7) {
                    file_buffer state{stateFile};
                    gbInst.load_state(state);
					render.display_message("State loaded", 2000);
                } else if (sym == SDLK_F5) {
                    memory_buffer buffer;
                    gbInst.save_state([&](uint32_t length) {
                        buffer.alloc(length);
                        return buffer;
                    });
                    std::ofstream fout(stateFile, std::ios::binary | std::ios::out);
                    fout.write(buffer, buffer.length());
                    fout.close();
                    buffer.dealloc();
					render.display_message("State saved", 2000);
                } else if (sym == SDLK_ESCAPE) {
                    endGame = true;
                }
				else if (sym == SDLK_SPACE) {
					runner.activate();
				}
                else if(sym >= SDLK_0 && sym <= SDLK_9)
                {
                    search_value *= 10;
                    search_value += (sym-SDLK_0);
                    std::cout << "Search value: " << search_value << std::endl;
                }
                else if (sym == SDLK_TAB) {
                    fast_forward = !fast_forward;
                    if (fast_forward) {
                        gbInst.setSpeed(9);
                        limitFunc = std::bind(limit, 1, std::placeholders::_1);
						render.display_message("Fast forward enabled", 2000);
                    }
                    else {
                        gbInst.setSpeed(0);
                        limitFunc = std::bind(limit, 16, std::placeholders::_1);
						render.display_message("Fast forward disabled", 2000);
                    }
                }
            }
        }

        limitFunc([&] {
            gbInst.tick();
        });
    }

    SDL_Quit();

    return 0;
}
