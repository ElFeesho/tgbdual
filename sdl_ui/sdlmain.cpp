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

#include <iostream>

#include <list>
#include "../gb_core/gb.h"

#include "resource.h"
#include "sdl_renderer.h"

#include <SDL.h>

#include "dialogs.h"

#include "multicast_transmitter.h"

#include "tcp_client.h"
#include "tcp_server.h"

class null_link_cable_source : public link_cable_source {
   public:
    uint8_t readByte() { return 0; }
    void sendByte(uint8_t data) {}
};

void cb_save_state(gb *g_gb, const std::string &file_name, int timer_state) {
    FILE *file = fopen(file_name.c_str(), "wb");
    g_gb->save_state(file);

    fseek(file, -100, SEEK_CUR);
    fwrite(&timer_state, 4, 1, file);
    fclose(file);
}

uint32_t cb_load_state(gb *g_gb, const std::string &file_name) {
    uint32_t timer_state = 0;

    FILE *file = fopen(file_name.c_str(), "rb");
    g_gb->restore_state(file);
    fseek(file, -100, SEEK_CUR);
    fread(&timer_state, 4, 1, file);
    fclose(file);
    return timer_state;
}

int main(int argc, char *argv[]) {

    bool fast_forward = false;

    link_cable_source *cable_source;
    if (argc == 1) {
        std::cerr << "Usage: " << argv[0] << " ROM-FILE [second-player]" << std::endl;
        return -1;
    } else if (argc == 2) {
        cable_source = new null_link_cable_source();
    } else if (argc == 3) {
        multicast_transmitter mc_transmitter{"239.0.10.0", 1337};
        mc_transmitter.transcieve([&](std::string addr) {
            std::cout << "Should connect to " << addr << std::endl;
            cable_source = new tcp_client(addr);
        });
    } else if (argc == 4) {
        cable_source = new tcp_server();
    }

    sdl_renderer render;

    std::string rom_file = argv[1];
    std::string sav_file = rom_file.substr(0, rom_file.find_last_of(".")) + ".sav";
    std::string save_state_file = rom_file.substr(0, rom_file.find_last_of(".")) + ".sv0";

    std::cout << "ROM File: " << rom_file << std::endl;
    std::cout << "SAV File: " << sav_file << std::endl;
    std::cout << "SV0 File: " << save_state_file << std::endl;

    bool endGame = false;

    gb g_gb = load_rom(argv[1], sav_file, &render, [&] {
        printf("Writing SRAM\n");
        save_sram(&g_gb, render.get_timer_state(), sav_file, g_gb.get_rom()->get_sram(), g_gb.get_rom()->get_info()->ram_size); }, [&]() { return cable_source->readByte(); }, [&](uint8_t data) { cable_source->sendByte(data); }
    );

    SDL_Event e;

    while (!endGame) {

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                endGame = true;
            } else if (e.type == SDL_KEYDOWN) {
                auto sym = e.key.keysym.sym;
                if (sym == SDLK_F7) {
                    render.set_timer_state(cb_load_state(&g_gb, save_state_file));
                } else if (sym == SDLK_F5) {
                    cb_save_state(&g_gb, save_state_file, render.get_timer_state());
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
