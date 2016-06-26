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
#include "setting.h"
#include "w32_posix.h"

#include <SDL.h>

#include "dialogs.h"


void in_directory(const char *dir, std::function<void()> dothis) {
    char cur_di[256];
    GetCurrentDirectory(256, cur_di);

    SetCurrentDirectory(dir);

    dothis();

    SetCurrentDirectory(cur_di);
}

void cb_save_state(gb *g_gb, const std::string &file_name, const char *save_dir, int timer_state) {
    in_directory(save_dir, [&]() {
        FILE *file = fopen(file_name.c_str(), "wb");
        g_gb->save_state(file);
        
        fseek(file, -100, SEEK_CUR);
        fwrite(&timer_state, 4, 1, file);
        fclose(file);
    });
}

uint32_t cb_load_state(gb *g_gb, const std::string &file_name, const char *save_dir) {
    uint32_t timer_state = 0;

    in_directory(save_dir, [&]() {
        FILE *file = fopen(file_name.c_str(), "rb");
        g_gb->restore_state(file);
        fseek(file, -100, SEEK_CUR);
        fread(&timer_state, 4, 1, file);
        fclose(file);
    });
    return timer_state;
}

int main(int argc, char *argv[]) {

    if (argc == 1)
    {
        std::cerr << "Usage: " << argv[0] << " ROM-FILE [second-player]" << std::endl; 
        return -1;
    }

    setting config;
    sdl_renderer render;
    
    std::string rom_file = argv[1];
    std::string sav_file = rom_file.substr(0, rom_file.find_last_of(".")) + ".sav";
    std::string save_state_file = rom_file.substr(0, rom_file.find_last_of(".")) + ".sv0";

    std::cout << "ROM File: " << rom_file << std::endl;
    std::cout << "SAV File: " << sav_file << std::endl;
    std::cout << "SV0 File: " << save_state_file << std::endl;

    char cur_dir[256];
    GetCurrentDirectory(256, cur_dir);

    char sv_dir[256];
    config.get_save_dir(sv_dir);

    load_key_config(&render, &config);

    key_dat fast_forward = {config.fast_forwerd[0], config.fast_forwerd[1]};

    bool endGame = false;

    SetCurrentDirectory(cur_dir);
    gb g_gb = load_rom(argv[1], &render, &config, argc == 3);
    
    SDL_Event e;

    while (!endGame) {

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                endGame = true;
            } else if (e.type == SDL_KEYDOWN) {
                auto sym = e.key.keysym.sym;
                if (sym == config.load_key[1]) {
                    render.set_timer_state(cb_load_state(&g_gb, save_state_file, sv_dir));
                } else if (sym == config.save_key[1]) {
                    cb_save_state(&g_gb, save_state_file, sv_dir, render.get_timer_state());
                }
            }
        }

        g_gb.run();

        if (!render.check_press(&fast_forward)) {
            g_gb.set_skip(config.frame_skip);

            if (config.speed_limit) {
                elapse_wait = (1000 << 16) / config.virtual_fps;
                elapse_time();
            }
        } else {
            g_gb.set_skip(config.fast_frame_skip);
            if (config.fast_speed_limit) {
                elapse_wait = (1000 << 16) / config.fast_virtual_fps;
                elapse_time();
            }
        }
    }

    if (g_gb.has_battery()) {
        save_sram(&g_gb, &config, render.get_timer_state(), sav_file, g_gb.get_rom()->get_sram(), g_gb.get_rom()->get_info()->ram_size);
    }

    return 0;
}
