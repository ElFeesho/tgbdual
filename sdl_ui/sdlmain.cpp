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

#include <gb.h>

#include <map>

#include "gameboy.h"

#include "sdl_renderer.h"

#include "null_link_source.h"

#include "multicast_transmitter.h"

#include "tcp_client.h"
#include "tcp_server.h"

#include "keyboard_input_source.h"
#include "joystick_input_source.h"

link_cable_source *processArguments(int *argc, char ***argv)
{
    int option = 0;
    link_cable_source *selected_cable_source = new null_link_source();
    while((option = getopt(*argc, *argv, "smc:")) != -1)
    {
        switch(option)
        {
            case 's':
            {
                cout << "Starting server" << endl;
                selected_cable_source = new tcp_server();
            }
            break;
            case 'm':
            {
                cout << "Broadcasting availability as client" << endl;
                multicast_transmitter mc_transmitter{"239.0.10.0", 1337};
                mc_transmitter.transcieve([&](std::string addr) {
                    std::cout << "Should connect to " << addr << std::endl;
                    selected_cable_source = new tcp_client(addr);
                });
            }
            break;
            case 'c':
                cout << "Connecting to " << optarg << endl;
                selected_cable_source = new tcp_client(optarg);
                break;
            case '?':
                if (optopt == 'c')
                {
                    cerr << "Target address must be passed in with -c" << endl;
                }
                else
                {
                    cerr << "Unknown option " << optopt << endl;
                }
                break;
        }
    }

    (*argc) -= optind;
    (*argv) += optind;

    return selected_cable_source;
}

uint32_t _elapsedWait{0x10AAAA};
void rateLimit()
{
    // Shouldn't be here
    static uint32_t lastdraw = 0, rest = 0;
    uint32_t t = SDL_GetTicks();

    rest = (rest & 0xffff) + _elapsedWait;

    uint32_t wait = rest >> 16;
    uint32_t elp = (uint32_t)(t - lastdraw);

    if (elp >= wait) {
        lastdraw = t;
        return;
    }

    if (wait - elp >= 4) {
        SDL_Delay(wait - elp - 3);
    }

    while ((SDL_GetTicks() - lastdraw) < wait);

    lastdraw += wait;
}

int main(int argc, char *argv[]) {

    if (argc == 1)
    {
        cerr << "Usage: " << argv[0] << " rom [-s|-m|-c client-address]" << endl;
        return 0;
    }

    link_cable_source *cable_source = processArguments(&argc, &argv);

    SDL_InitSubSystem(SDL_INIT_JOYSTICK);

    if (SDL_NumJoysticks() > 0) {
        // Open joystick
        auto joy = SDL_JoystickOpen(0);

        if (joy) {
            printf("Opened Joystick 0\n");
            printf("Number of Axes: %d\n", SDL_JoystickNumAxes(joy));
            printf("Number of Buttons: %d\n", SDL_JoystickNumButtons(joy));
            printf("Number of Balls: %d\n", SDL_JoystickNumBalls(joy));
        } else {
            printf("Couldn't open Joystick 0\n");
        }
    }

    bool fast_forward = false;

    sdl_renderer render;

    std::string rom_file{argv[0]};

    struct stat buffer;   
    if (stat (rom_file.c_str(), &buffer) != 0)
    {
        cerr << "Rom file " << rom_file << " is not accessible" << endl;
        return -1;
    }

    bool endGame = false;

    gameboy gbInst{&render, cable_source};
    gbInst.load_rom(rom_file);

    SDL_Event e;

    keyboard_input_source keyboardSource;
    joystick_input_source joystickSource;

    std::map<int, input_source*> input_sources;
    input_sources[SDL_KEYDOWN] = &keyboardSource;
    input_sources[SDL_KEYUP] = &keyboardSource;
    input_sources[SDL_JOYBUTTONDOWN] = &joystickSource;
    input_sources[SDL_JOYBUTTONUP] = &joystickSource;
    input_sources[SDL_JOYAXISMOTION] = &joystickSource;
    
    while (!endGame) {

        while (SDL_PollEvent(&e)) {

            if (input_sources.find(e.type) != input_sources.end())
            {
                gbInst.provideInput([&](uint8_t initialState) -> uint8_t {
                    return input_sources[e.type]->provide_input(initialState, e);
                });
            }

            if (e.type == SDL_QUIT) {
                endGame = true;
            } else if (e.type == SDL_KEYDOWN) {
                auto sym = e.key.keysym.sym;
                if (sym == SDLK_F7) {
                    gbInst.load_state();
                } else if (sym == SDLK_F5) {
                    gbInst.save_state();
                } else if (sym == SDLK_ESCAPE) {
                    endGame = true;
                }
                else if(sym == SDLK_TAB)
                {
                    fast_forward = !fast_forward;
                    if (fast_forward)
                    {
                        gbInst.fastForward();
                        _elapsedWait = (1000 << 16) / 999;
                    }
                    else
                    {
                        gbInst.normalForward();
                        _elapsedWait = (1000 << 16) / 60;
                    }
                }
            }
        }

        gbInst.tick();
        rateLimit();
    }

    return 0;
}
