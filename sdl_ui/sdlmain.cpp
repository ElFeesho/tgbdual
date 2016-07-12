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
#include "sdl_gamepad_source.h"

#include <fstream>

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

void limit(uint32_t targetTime, std::function<void()> operation)
{
    uint32_t startTime = SDL_GetTicks();

    operation();

    uint32_t operationTime = SDL_GetTicks()-startTime;

    if (operationTime < targetTime)
    {
        SDL_Delay(targetTime - operationTime);
    }
}

class buffer
{
public:
    virtual operator const char*() = 0;
    virtual operator uint8_t*() = 0;
    virtual uint32_t length() = 0;
};

class file_buffer : buffer 
{
public:
    file_buffer(const std::string &name)
    {
        ifstream _file{name, std::ios::binary | std::ios::in};

        if (!_file.good()) {
            throw std::domain_error("Could not open "+name);
        }

        _file.seekg(0, _file.end);
        _length = _file.tellg();
        _file.seekg(0, _file.beg);
        _buffer = new uint8_t[_length];

        _file.read((char*)_buffer, _length);
        _file.close();
    }

    ~file_buffer() 
    {
        delete[] _buffer;
    }

    uint32_t length()
    {
        return _length;
    }

    operator const char*() {
        return (const char *)_buffer;
    }

    operator uint8_t*() {
        return _buffer;
    }
    
private:
    uint32_t _length { 0 };
    uint8_t *_buffer;
};

class memory_buffer : buffer
{
public:
    void alloc(uint32_t length)
    {
        _length = length;
        _buffer = new uint8_t[length];
    }

    void dealloc()
    {
        delete[] _buffer;
    }

    uint32_t length() override {
        return _length;
    }

    operator const char*() override {
        return (const char *)_buffer;
    }

    operator uint8_t*() override {
        return _buffer;
    }
private:
    uint32_t _length;
    uint8_t *_buffer { nullptr };
};

int main(int argc, char *argv[]) {

    if (argc == 1)
    {
        cerr << "Usage: " << argv[0] << " rom [-s|-m|-c client-address]" << endl;
        return 0;
    }

    link_cable_source *cable_source = processArguments(&argc, &argv);

    

    bool fast_forward = false;

    sdl_renderer render;
    sdl_gamepad_source gp_source;

    std::string romFilename{argv[0]};
    std::string saveFile = romFilename.substr(0, romFilename.find_last_of(".")) + ".sav";
    std::string stateFile = romFilename.substr(0, romFilename.find_last_of(".")) + ".sv0";

    struct stat buffer;   
    if (stat (romFilename.c_str(), &buffer) != 0)
    {
        cerr << "Rom file " << romFilename << " is not accessible" << endl;
        return -1;
    }

    bool endGame = false;

    gameboy gbInst{&render, &gp_source, cable_source};
    
    file_buffer romBuffer{romFilename};
    file_buffer saveBuffer{saveFile};
    if (stat (saveFile.c_str(), &buffer) != 0)
    {
        gbInst.load_rom(romBuffer, romBuffer.length(), nullptr, 0);    
    }
    else
    {
        gbInst.load_rom(romBuffer, romBuffer.length(), saveBuffer, saveBuffer.length());
    }

    SDL_Event e;
    
    std::function<void(std::function<void()>)> limitFunc = std::bind(limit, 16, std::placeholders::_1);

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
                } else if (sym == SDLK_ESCAPE) {
                    endGame = true;
                }
                else if(sym == SDLK_TAB)
                {
                    fast_forward = !fast_forward;
                    if (fast_forward)
                    {
                        gbInst.setSpeed(9);
                        limitFunc = std::move(std::bind(limit, 1, std::placeholders::_1));
                    }
                    else
                    {
                        gbInst.setSpeed(0);
                        limitFunc = std::bind(limit, 16, std::placeholders::_1);
                    }
                }
            }
        }

        limitFunc([&]{
            gbInst.tick();
        });
    }

    SDL_Quit();

    return 0;
}
