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

#include <gb.h>

#include "sdl_renderer.h"

#include "null_link_source.h"

#include "multicast_transmitter.h"

#include "tcp_client.h"
#include "tcp_server.h"

uint8_t *file_read(const std::string &name, int *size) {
    uint8_t *dat = 0;
    FILE *file = fopen(name.c_str(), "rb");
    if (!file)
        return nullptr;
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);
    dat = (uint8_t *)malloc(*size);
    fread(dat, 1, *size, file);
    fclose(file);
    return dat;
}

void save_sram(gb *g_gb, const std::string &file_name, uint8_t *buf, int size) {
    FILE *fsu = fopen(file_name.c_str(), "w");
    fwrite((void*)buf, size, 1, fsu);
    fclose(fsu);
}

gb load_rom(const std::string &romFile, const std::string &save_file, sdl_renderer *render, std::function<void()> save_cb, std::function<uint8_t()> link_read_cb, std::function<void(uint8_t)> link_write_cb) {
    
    int size = 0;
    uint8_t *dat = file_read(romFile, &size);
    
    int ram_size;
    uint8_t *ram = file_read(save_file, &ram_size);
    
    gb g_gb{render, save_cb, link_read_cb, link_write_cb};
    g_gb.load_rom(dat, size, ram, ram_size);

    free(dat);
    free(ram);

    SDL_WM_SetCaption(g_gb.get_rom()->get_info()->cart_name, 0);

    return g_gb;
}


static int elapse_wait = 0x10AAAA;

static void elapse_time(void) {
    static uint32_t lastdraw = 0, rest = 0;
    uint32_t t = SDL_GetTicks();

    rest = (rest & 0xffff) + elapse_wait;

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


void cb_save_state(gb *g_gb, const std::string &file_name) {
    FILE *file = fopen(file_name.c_str(), "wb");
    g_gb->save_state(file);
    fclose(file);
}

void cb_load_state(gb *g_gb, const std::string &file_name) {
    FILE *file = fopen(file_name.c_str(), "rb");
    g_gb->restore_state(file);
    fclose(file);
}

int main(int argc, char *argv[]) {

    link_cable_source *cable_source;

    if (argc == 1)
    {
        cerr << "Usage: " << argv[0] << " rom [-s|-m|-c client-address]" << endl;
        return 0;
    }

    int option = 0;
    while((option = getopt(argc, argv, "smc:")) != -1)
    {
        switch(option)
        {
            case 's':
            {
                cout << "Starting server" << endl;
                cable_source = new tcp_server();
            }
            break;
            case 'm':
            {
                cout << "Broadcasting availability as client" << endl;
                multicast_transmitter mc_transmitter{"239.0.10.0", 1337};
                mc_transmitter.transcieve([&](std::string addr) {
                    std::cout << "Should connect to " << addr << std::endl;
                    cable_source = new tcp_client(addr);
                });
            }
            break;
            case 'c':
                cout << "Connecting to " << optarg << endl;
                cable_source = new tcp_client(optarg);
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
                return -1;
                break;
        }
    }

    argc -= optind;
    argv += optind;

    for (int i = 0; i < argc; i++)
    {
        cout << argv[i] << endl;
    }

    bool fast_forward = false;

    sdl_renderer render;

    std::string rom_file{argv[0]};
    std::string sav_file = rom_file.substr(0, rom_file.find_last_of(".")) + ".sav";
    std::string save_state_file = rom_file.substr(0, rom_file.find_last_of(".")) + ".sv0";

    bool endGame = false;

    gb g_gb = load_rom(rom_file, sav_file, &render, [&] {
        printf("Writing SRAM\n");
        save_sram(&g_gb, sav_file, g_gb.get_rom()->get_sram(), g_gb.get_rom()->get_info()->ram_size); 
    }, 
    [&]() { 
        return cable_source->readByte(); 
    }, 
    [&](uint8_t data) { 
        cable_source->sendByte(data); 
    }
    );

    SDL_Event e;

    while (!endGame) {

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                endGame = true;
            } else if (e.type == SDL_KEYDOWN) {
                auto sym = e.key.keysym.sym;
                if (sym == SDLK_F7) {
                    cb_load_state(&g_gb, save_state_file);
                } else if (sym == SDLK_F5) {
                    cb_save_state(&g_gb, save_state_file);
                } else if (sym == SDLK_ESCAPE) {
                    endGame = true;
                }
                else if(sym == SDLK_RIGHT)
                {
                    render.set_pad(render.check_pad() | 0x80);
                }
                else if(sym == SDLK_UP)
                {
                    render.set_pad(render.check_pad() | 0x20);
                }
                else if(sym == SDLK_DOWN)
                {
                    render.set_pad(render.check_pad() | 0x10);
                }
                else if(sym == SDLK_LEFT)
                {
                    render.set_pad(render.check_pad() | 0x40);
                }
                else if(sym == SDLK_z)
                {
                    render.set_pad(render.check_pad() | 0x01);
                }
                else if(sym == SDLK_x)
                {
                    render.set_pad(render.check_pad() | 0x02);
                }
                else if(sym == SDLK_RSHIFT)
                {
                    render.set_pad(render.check_pad() | 0x04);
                }
                else if(sym == SDLK_RETURN)
                {
                    render.set_pad(render.check_pad() | 0x08);
                }
                else if(sym == SDLK_TAB)
                {
                    fast_forward = !fast_forward;
                }
            }
            else if (e.type == SDL_KEYUP){
                auto sym = e.key.keysym.sym;
                if(sym == SDLK_RIGHT)
                {
                    render.set_pad(render.check_pad() - 0x80);
                } else if(sym == SDLK_UP)
                {
                    render.set_pad(render.check_pad() - 0x20);
                }
                else if(sym == SDLK_DOWN)
                {
                    render.set_pad(render.check_pad() - 0x10);
                }
                else if(sym == SDLK_LEFT)
                {
                    render.set_pad(render.check_pad() - 0x40);
                }
                else if(sym == SDLK_z)
                {
                    render.set_pad(render.check_pad() - 0x01);
                }
                else if(sym == SDLK_x)
                {
                    render.set_pad(render.check_pad() - 0x02);
                }
                else if(sym == SDLK_RSHIFT)
                {
                    render.set_pad(render.check_pad() - 0x04);
                }
                else if(sym == SDLK_RETURN)
                {
                    render.set_pad(render.check_pad() - 0x08);
                }
            }

        }

        g_gb.run();

        if (!fast_forward) {
            g_gb.set_skip(0);
            elapse_wait = (1000 << 16) / 60;
            elapse_time();
        } else {
            g_gb.set_skip(9);
            elapse_wait = (1000 << 16) / 999;
            elapse_time();        
        }
    }

    return 0;
}
